class Grammar:

    def __init__(self, N, T, P, S):
        self.N = N
        self.T = T
        self.P = P
        self.S = S

    def __str__(self):
        return ('N: ' + str(self.N)
                + '\nT: ' + str(self.T)
                + '\nP: ' + str(self.P)
                + '\nS: ' + str(self.S))


class PDA:

    def __init__(self, Q, T, Γ, δ, q0, Z0, F):
        self.Q = Q
        self.T = T
        self.Γ = Γ
        self.δ = δ
        self.q0 = q0
        self.Z0 = Z0
        self.F = F

    def __str__(self):
        return ('Q: ' + str(self.Q)
                + '\nT: ' + str(self.T)
                + '\nΓ: ' + str(self.Γ)
                + '\nδ: ' + str(self.δ)
                + '\nq0: ' + str(self.q0)
                + '\nZ0: ' + str(self.Z0)
                + '\nF: ' + str(self.F))


def gen_rhs(Q, m, a, B, P, q, A):
    if len(Q) == m + 1:
        rhs = a + ''.join(f"[{Q[i]},{B[i]},{Q[i + 1]}]" for i in range(m))
        P.setdefault(f"[{q},{A},{Q[-1]}]", set()).add(rhs)
    else:
        for p in M.Q:
            gen_rhs(Q + [p], m, a, B, P, q, A)
    return P


def PDA2CFG(M):
    N = {f'[{q},{A},{p}]' for q in M.Q for p in M.Q for A in M.Γ} | {'S'}

    T = M.T

    P = {'S': {f"[q0,Z,{q}]" for q in M.Q}}

    for (q, a, A) in M.δ:
        for (p, B) in M.δ[(q, a, A)]:
            m = len(list(B))
            for _ in M.Q:
                if m == 0:
                    P.setdefault(f"[{q},{A},{p}]", set()).add(a)
                else:
                    P = gen_rhs([p], m, a, B, P, q, A)

    S = 'S'
    return Grammar(N, T, P, S)


print("请输入状态(空格分隔):")
Q = set(input().strip().split())
print("请输入终结符(空格分隔):")
T = set(input().strip().split())
print("请输入栈符(空格分隔):")
Γ = set(input().strip().split())
print("请输入产生式数:")
p = int(input())
δ = {}
for _ in range(p):
    p = []
    print("请输入产生式（格式为 Q,T,Γ → Q,Γ，QTΓ 和 QΓ* 用空格分隔,em代表空）:")
    production = input().strip().split()
    for t in range(len(production)):
        t1 = production[t].split(',')
        for i in range(len(t1)):
            if t1[i] == 'em':
                t1[i] = ''
        t1 = tuple(t1)
        p.append(t1)
    # print(p)

    A = p[0]
    alpha = p[1:] if len(production) >= 2 else ['']
    if A not in δ:
        δ[A] = set()
    δ[A].update(alpha)


# 消去 ε 生成式
def algorithm_3(G):
    G1 = Grammar(G.N, G.T, {}, G.S)
    G1.N = {A for A in G.N if A in G.P}

    # 1. 找出 N' = {A|A ∈ N 且 A ⇒ ε}
    N_0 = None
    N_prime = {A for A in G1.N if '' in G.P[A]}

    while N_0 != N_prime:
        N_0 = N_prime
        N_prime |= {A for A in G1.N if N_prime & G.P[A]}

    # 2. 对于生成式 A → αCβ, 若 C ∈ N', α,β ∉ N', 则加入 A → αYβ, 其中 Y 为 C 或 ε
    for A in G1.N:
        res = set()
        for s in G.P[A]:
            if s == '':
                continue
            indices = [i for i, c in enumerate(s) if c in N_prime]
            n = len(indices)

            for mask in range(0, 1 << n):
                cur = list(s)
                for i in range(n):
                    if mask & (1 << i):
                        cur[indices[i]] = ''
                res.add(''.join(cur))
        G1.P[A] = res

    # 3. 若 S ∈ N', 则加入 S1 → ε|S
    if 'S' in N_prime:
        G1.P['S1'] = {'', 'S'}
        G1.N |= {'S1'}
        G1.S = 'S1'

    return G1


# 消去单生成式
def algorithm_4(G):
    G1 = Grammar(G.N, G.T, {}, G.S)
    for A in G.N:
        # 1. 对每个非终结符 A ∈ N , 构造 N_A = {B|A ⇒ B}
        N_0 = {A}
        N_prime = {C for C in G.N for B in N_0 if C in G.P[B]} | N_0
        while N_prime != N_0:
            N_0 = N_prime
            N_prime = {C for C in G.N for B in N_0 if C in G.P[B]} | N_0
        N_A = N_prime

        # 2. 如果 B → α ∈ P 且不是单生成式，则对于 B ∈ N_A 中的所有 A , 加入 A → α
        G1.P[A] = {s for B in N_A for s in G.P[B] if s not in G.N}

    return G1


# 消去无用符号
def algorithm_1(G):
    N_0 = None
    N_prime = {A for A in G.N for s in G.P[A] if not G.N & set(s)}

    while N_0 != N_prime:
        N_0 = N_prime
        N_prime |= {A for A in G.N for s in G.P[A] if N_0 & set(s)}

    N_1 = N_prime

    return Grammar(N_1, G.T, {A: {s for s in G.P[A] if not (G.N - N_1) & set(s)} for A in N_1}, G.S)


def algorithm_2(G):
    N_0 = {G.S}
    N_prime = {X for X in G.N | G.T for A in N_0 for s in G.P[A] if X in s} | N_0
    while N_0 != N_prime:
        N_0 = N_prime
        N_prime = {X for X in G.N | G.T for A in N_0 & G.N for s in G.P[A] if X in s} | N_0
    N_1 = N_prime & G.N
    T_1 = N_prime & G.T

    return Grammar(N_1, T_1, {A: {s for s in G.P[A] if not (G.N - N_1) & set(s)} for A in N_1}, G.S)


print("PDA to CFG conversion result:")
M = PDA(
    Q,
    T,
    Γ,
    δ,
    q0='q0',
    Z0='Z0',
    F=set()
)

'''
q0 q1
a b
B Z
6
q0,b,Z q0,BZ
q0,b,B q0,BB
q0,a,B q1,em
q1,a,B q1,em
q1,em,B q1,em
q1,em,Z q1,em
ε
'''

print(PDA2CFG(M))
print(algorithm_2(algorithm_1(algorithm_4(algorithm_3(PDA2CFG(M))))))

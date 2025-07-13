class Grammar:

    def __init__(self, N, T, P, S):
        self.N = N
        self.T = T
        self.P = P
        self.S = S

    def __str__(self):
        return 'N: ' + str(self.N) + '\nT: ' + str(self.T) + '\nP: ' + str(self.P) + '\nS: ' + str(self.S)


# 消去 ε 生成式
def algorithm_3(G):
    G1 = Grammar(G.N, G.T, {}, G.S)

    # 1. 找出 N' = {A|A ∈ N 且 A ⇒ ε}
    N_0 = None
    N_prime = {A for A in G.N if '' in G.P[A]}

    while N_0 != N_prime:
        N_0 = N_prime
        N_prime |= {A for A in G.N if N_prime & G.P[A]}

    # 2. 对于生成式 A → αCβ, 若 C ∈ N', α,β ∉ N', 则加入 A → αYβ, 其中 Y 为 C 或 ε
    for A in G.N:
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


G = Grammar(
    N={'S', 'A', 'B', 'C', 'D'},
    T={'a', 'b', 'c', 'd'},
    P={
        'S': {'a', 'bA', 'B', 'ccD'},
        'A': {'abB', ''},
        'B': {'aA'},
        'C': {'ddC'},
        'D': {'ddd'}
    },
    S='S'
)

print(G)
print(algorithm_3(G))
print(algorithm_4(algorithm_3(G)))
print(algorithm_1(algorithm_4(algorithm_3(G))))
print(algorithm_2(algorithm_1(algorithm_4(algorithm_3(G)))))
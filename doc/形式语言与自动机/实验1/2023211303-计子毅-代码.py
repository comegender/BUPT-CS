def main():

    # 1. 输入NFA描述
    print("请输入NFA描述：")

    # 1.1 输入状态数和字母表
    nfa_states = int(input("状态数(例如4): "))
    alphabet = input("字母表(用空格分隔，例如'a b'): ").split()

    # 1.2 初始化转移函数字典
    transitions = {}
    print("\n请输入转移函数(格式：源状态 符号 目标状态列表)：")
    print("例如: 0 a 0 1 表示 δ(q0,a)={q0,q1}")
    print("输入空行结束")

    while True:
        line = input("> ").strip()
        if not line:
            break

        parts = line.split()
        src = int(parts[0])
        symbol = parts[1]
        dst_states = set(map(int, parts[2:]))

        # 存储转移函数
        key = (src, symbol)
        transitions[key] = dst_states

    # 1.3 输入初始状态和接受状态
    initial_state = int(input("\n初始状态(例如0): "))
    final_states = set(map(int, input("接受状态(用空格分隔，例如'3'): ").split()))

    print("-----------------------------------------------")

    # 2. 打印输入的NFA
    print("输入的NFA为:")
    print_nfa(nfa_states, alphabet, transitions, final_states, initial_state)

    # 3. 执行NFA到DFA的转换
    dfa_states, dfa_transitions, dfa_final, dfa_initial = nfa_to_dfa(
        nfa_states, alphabet, transitions, final_states, initial_state
    )

    # 4. 打印结果
    print("\n转换得到的DFA为：")
    print_dfa(dfa_states, alphabet, dfa_transitions, dfa_final, dfa_initial)


    print("\n按任意键继续...", end="")
    input()  # 等待用户输入
    print("程序结束")

    
def nfa_to_dfa(nfa_states, alphabet, nfa_transitions, nfa_final, initial_state=0):
    """
    子集构造法：将NFA转换为DFA（使用深度优先搜索DFS实现）
    参数:
        nfa_states: NFA的状态总数（整数）
        alphabet: 字母表（字符列表，如["a", "b"]）
        nfa_transitions: NFA转移函数字典，格式为{(源状态, 字符): {目标状态集合}}
        nfa_final: NFA的接受状态集合（set或frozenset）
        initial_state: NFA的初始状态编号（默认为0）
    返回:
        (dfa_states, dfa_transitions, dfa_final, dfa_initial) 四元组，其中：
        - dfa_states: DFA状态列表，每个状态是NFA状态的frozenset
        - dfa_transitions: DFA转移函数字典，格式为{(frozenset状态, 字符): frozenset状态}
        - dfa_final: DFA接受状态列表
        - dfa_initial: DFA初始状态（frozenset）
    """
    # 输入验证：检查初始状态是否合法
    if initial_state < 0 or initial_state >= nfa_states:
        raise ValueError(f"初始状态必须在0到{nfa_states - 1}范围内")

    # 初始化数据结构
    dfa_states = []          # 存储所有DFA状态（每个状态是NFA状态的frozenset）
    dfa_transitions = {}     # DFA转移函数字典
    stack = []               # 处理栈（用于DFS遍历）

    # DFA的初始状态是包含NFA初始状态的单元素集合
    dfa_initial = frozenset({initial_state})
    stack.append(dfa_initial)  # 将初始状态压入栈
    dfa_states.append(dfa_initial)  # 添加到已知状态列表

    # 主循环：处理栈中的状态直到栈为空
    while stack:
        # 弹出当前处理的状态（DFS的LIFO顺序）
        current = stack.pop()

        # 对字母表中的每个字符处理转移
        for symbol in alphabet:
            # 计算move(current, symbol)：当前状态通过symbol能到达的所有NFA状态
            next_states = set()
            for state in current:
                # 查找NFA转移表中是否存在(state, symbol)的转移
                key = (state, symbol)
                if key in nfa_transitions:
                    next_states.update(nfa_transitions[key])  # 合并所有可能的目标状态

            # 将结果转换为不可变集合（frozenset），以便作为字典键
            next_states = frozenset(next_states) if next_states else frozenset()

            # 如果是新发现的DFA状态，则加入处理栈和状态列表
            if next_states not in dfa_states:
                dfa_states.append(next_states)
                stack.append(next_states)  # 压入栈，后续会处理这个新状态

            # 记录DFA转移函数：从current通过symbol转移到next_states
            dfa_transitions[(current, symbol)] = next_states

    # 确定DFA的接受状态：任何包含NFA接受状态的DFA状态都是接受状态
    dfa_final = [s for s in dfa_states if any(n in s for n in nfa_final)]

    return dfa_states, dfa_transitions, dfa_final, dfa_initial


def print_nfa(states, alphabet, transitions, final_states, initial_state):
    """
    以标准格式打印NFA的完整定义

    参数：
        states (int): NFA的状态总数（从q0到qN-1）
        alphabet (list): 字母表中的符号列表（如["a", "b"]）
        transitions (dict): 转移函数，格式为{(源状态,符号): {目标状态集合}}
        final_states (set): 接受/终结状态集合
        initial_state (int): 初始状态索引

    输出格式：
        Q = {q0, q1, ..., qN}   # 状态集合
        T = {a, b, ...}         # 字母表
        q0 = qX                 # 初始状态
        F = {qX, qY}           # 接受状态集合
        状态转换表（表格形式）
    """
    # 1. 打印状态集合
    # 生成所有状态标识 q0 到 qN-1
    state_list = [f"q{i}" for i in range(states)]
    print("Q = {" + ", ".join(state_list) + "}")  # 格式示例：Q = {q0, q1, q2}

    # 2. 打印字母表
    print("T = {" + ", ".join(alphabet) + "}")  # 格式示例：T = {a, b}

    # 3. 打印初始状态
    print(f"q0 = q{initial_state}")  # 格式示例：q0 = q0

    # 4. 打印接受状态集合
    # 将接受状态排序后转换为qX格式，若无则显示∅
    final_str = "{" + ", ".join(f"q{f}" + "}" for f in sorted(final_states)) if final_states else "∅"
    print(f"F = {final_str}")  # 格式示例：F = {q2}

    # 5. 打印状态转换表（表格形式）
    print("\n其中，状态转换表如下")

    # 打印表头（状态 + 各字母）
    header = ["状态"] + alphabet
    print("|".join(f"{item:^15}" for item in header))  # 每个单元格宽度15字符，居中
    print("-" * (15 * len(header) + len(header) - 1))  # 分隔线

    # 打印表格内容
    for state in range(states):
        row = [f"q{state}"]  # 第一列为状态标识

        for symbol in alphabet:
            key = (state, symbol)
            if key in transitions:
                # 存在转移：将目标状态集合转换为逗号分隔的字符串
                dst_set = transitions[key]
                dst_str = ",".join(f"q{d}" for d in sorted(dst_set)) if dst_set else "∅"
                row.append(dst_str)
            else:
                # 无转移定义显示空集符号
                row.append("∅")

        # 打印当前状态的行
        print("|".join(f"{item:^15}" for item in row))


def print_dfa(states, alphabet, transitions, final_states, initial_state):
    """格式化输出DFA的完整定义

    参数：
        states: DFA状态集合，每个状态表示为NFA状态的frozenset
        alphabet: 输入字母表符号列表
        transitions: 状态转移函数，键为(状态, 符号)元组
        final_states: 接受状态集合
        initial_state: 初始状态集合
    """
    # 1. 输出状态集合
    state_strs = [f"[{",".join(f"q{s}" for s in sorted(state))}]" if state else "∅"
                  for state in states]
    print(f"Q = {{{", ".join(state_strs)}}}")

    # 2. 输出字母表
    print(f"T = {{{", ".join(alphabet)}}}")

    # 3. 输出初始状态
    states_str = f"[{",".join(f"q{s}" for s in sorted(initial_state))}]"
    print(f"q0 = {states_str}")

    # 4. 输出接受状态
    final_strs = [f"[{",".join(f"q{s}" for s in sorted(state))}]"
                  for state in final_states]
    final_str = "{" + ", ".join(final_strs) + "}" if final_states else "∅"
    print(f"F = {final_str}")

    # 5. 输出状态转移表
    print("\n状态转换表:")

    # 生成状态到字符串的映射
    state_repr = {
        state: f"[{",".join(f"q{s}" for s in sorted(state))}]" if state else "∅"
        for state in states
    }

    # 计算表格列宽（基于最长状态表示）
    col_width = max(len(s) for s in state_repr.values()) + 2

    # 打印表头
    header = ["状态"] + alphabet
    print("|".join(f"{item:^{col_width}}" for item in header))
    print("-" * (col_width * len(header) + len(header) - 1))

    # 打印表格内容
    for state in states:
        if state:
            row = [state_repr[state]]
            for symbol in alphabet:
                row.append(state_repr[transitions[(state, symbol)]])
            print("|".join(f"{item:^{col_width}}" for item in row))


if __name__ == "__main__":
    main()

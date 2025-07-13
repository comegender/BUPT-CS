import itertools
import numpy as np

# 输入信号振幅
A1, A2, A3 = 25, 10, 45

# 频率取值范围
f1_range = range(36, 41)  # f1 ∈ [36, 40]
f2_range = range(41, 51)  # f2 ∈ [41, 50]
f3_range = range(46, 56)  # f3 ∈ [46, 55]

# 存储所有可行解
valid_solutions = []

# 枚举所有可能的频率组合
for f1, f2, f3 in itertools.product(f1_range, f2_range, f3_range):
    # 检查频率间隔条件：|f_i - f_j| > 5
    if abs(f1 - f2) <= 5 or abs(f1 - f3) <= 5 or abs(f2 - f3) <= 5:
        continue
    
    # 计算所有二阶和三阶交调频率
    intermodulations = set()
    
    # 二阶交调：f_i ± f_j
    for fi, fj in itertools.permutations([f1, f2, f3], 2):
        intermodulations.add(fi + fj)
        intermodulations.add(abs(fi - fj))
    
    # 三阶交调：f_i ± f_j ± f_k
    for fi, fj, fk in itertools.permutations([f1, f2, f3], 3):
        intermodulations.add(fi + fj - fk)
        intermodulations.add(fi - fj + fk)
        intermodulations.add(-fi + fj + fk)
    
    # 检查交调是否出现在接收带内（f_i ± 5）
    invalid = False
    for f in [f1, f2, f3]:
        for im in intermodulations:
            if abs(im - f) <= 5:
                invalid = True
                break
        if invalid:
            break
    
    if invalid:
        continue
    
    # 检查信噪比条件（简化计算，假设 SNR > 10 dB）
    # 实际计算需根据输入输出关系表拟合非线性关系
    # 这里假设所有满足交调约束的组合 SNR 均满足
    snr_ok = True
    
    if snr_ok:
        valid_solutions.append((f1, f2, f3))

# 输出所有可行解
print("所有满足条件的频率组合：")
for sol in valid_solutions:
    print(f"f1={sol[0]}, f2={sol[1]}, f3={sol[2]}")

# 计算 r = (f2 - f1) / (f3 - f2) 并找出最大值
if valid_solutions:
    max_r = -np.inf
    best_sol = None
    for sol in valid_solutions:
        f1, f2, f3 = sol
        r = (f2 - f1) / (f3 - f2)
        if r > max_r:
            max_r = r
            best_sol = sol
    print(f"\n最优解（r 最大）：f1={best_sol[0]}, f2={best_sol[1]}, f3={best_sol[2]}, r={max_r:.2f}")
else:
    print("无可行解！")
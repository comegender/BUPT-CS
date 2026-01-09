import numpy as np
import pandas as pd
from scipy.optimize import differential_evolution
import math

# ------------------- 参数设定 -------------------
uav_count = 5
smoke_per_uav = 3
smoke_burn = 8.0       # 每颗烟幕弹持续时间
min_gap = 0.5          # 同一无人机相邻投放间隔 ≥ 0.5s

# 导弹参数
G = 9.8  # 重力加速度
V_SINK = 3.0  # 烟云下沉速度

# 定义无人机初始位置（示例值，根据实际情况调整）
UAV_INIT_POSITIONS = {
    0: np.array([17800.0, 0.0, 1800.0]),
    1: np.array([12000.0, 1400.0, 1400.0]),
    2: np.array([6000.0, -3000.0, 700.0]),
    3: np.array([9000.0, 2000.0, 1500.0]),
    4: np.array([11000.0, -1000.0, 1300.0])
}

# 目标位置（示例值）
P_TARGET_REAL = np.array([0.0, 200.0, 5.0])

# 三枚导弹出现时间区间，覆盖整个作战窗口
missile_intervals = [
    (0.0, 10.0),   # 第1枚导弹
    (10.0, 20.0),  # 第2枚导弹
    (20.0, 30.0)   # 第3枚导弹
]

# ------------------- 覆盖时间计算 -------------------
def compute_coverage(x):
    events = []
    for u in range(uav_count):
        # 获取无人机的速度和方向
        speed_idx = (u * (smoke_per_uav * 2 + 2))
        angle_idx = speed_idx + 1
        
        # 获取该无人机的投放时间
        rel_times = []
        for k in range(smoke_per_uav):
            idx = speed_idx + 2 + (k * 2)
            t_rel = x[idx]
            t_fuze = x[idx+1]
            t_burst = t_rel + t_fuze
            rel_times.append(t_burst)
            events.append((u, k, t_burst))
        rel_times.sort()
        for i in range(1, len(rel_times)):
            if rel_times[i] - rel_times[i-1] < min_gap:
                return -1e6  # 违反间隔约束，惩罚

    events.sort(key=lambda e: e[2])
    cover_total = 0.0
    for (start, end) in missile_intervals:
        intervals = []
        for (_, _, tb) in events:
            s = tb
            e = tb + smoke_burn
            if e < start or s > end:
                continue
            intervals.append([max(s, start), min(e, end)])
        if not intervals:
            continue
        intervals.sort()
        merged = [intervals[0]]
        for seg in intervals[1:]:
            if seg[0] <= merged[-1][1]:
                merged[-1][1] = max(merged[-1][1], seg[1])
            else:
                merged.append(seg)
        cover_total += sum(e - s for s, e in merged)
    return cover_total

def obj(x):
    return -compute_coverage(x)

# ------------------- 决策变量范围 -------------------
bounds = []
for u in range(uav_count):
    # 每台无人机的速度和方向
    bounds.append((100.0, 140.0))  # 速度 (m/s)
    bounds.append((0.0, 2 * np.pi))  # 方向 (弧度)
    
    # 每颗烟幕弹的投放时间和引信时间
    for k in range(smoke_per_uav):
        bounds.append((0.0, 30.0))  # 投放时刻
        bounds.append((0.5, 3.0))   # 引信延时

# ------------------- 计算单个烟幕弹的遮蔽信息 -------------------
def calculate_smoke_info(x, uav_idx, smoke_idx):
    # 获取该无人机的速度和方向
    speed_idx = (uav_idx * (smoke_per_uav * 2 + 2))
    angle_idx = speed_idx + 1
    speed = x[speed_idx]
    angle = x[angle_idx]
    
    # 获取烟幕弹的投放时间和引信时间
    idx = speed_idx + 2 + (smoke_idx * 2)
    t_rel = x[idx]
    t_fuze = x[idx+1]
    t_burst = t_rel + t_fuze
    
    # 计算投放坐标
    direction = np.array([np.cos(angle), np.sin(angle), 0.0])
    init_pos = UAV_INIT_POSITIONS[uav_idx]
    release_pos = init_pos + speed * t_rel * direction
    
    # 计算爆炸坐标（考虑自由落体）
    blast_pos = release_pos + speed * t_fuze * direction
    blast_pos[2] -= 0.5 * G * t_fuze**2
    
    # 计算单个遮蔽时长
    smoke_interval = (t_burst, t_burst + smoke_burn)
    cover_time = 0.0
    affected_missiles = []
    
    for missile_idx, (missile_start, missile_end) in enumerate(missile_intervals):
        # 计算时间重叠
        start_overlap = max(smoke_interval[0], missile_start)
        end_overlap = min(smoke_interval[1], missile_end)
        overlap_duration = max(0.0, end_overlap - start_overlap)
        
        if overlap_duration > 0:
            cover_time += overlap_duration
            affected_missiles.append(missile_idx + 1)  # 导弹编号从1开始
    
    return {
        'speed': speed,
        'angle_deg': np.degrees(angle),
        'release_pos': release_pos,
        'blast_pos': blast_pos,
        'cover_time': cover_time,
        'affected_missiles': affected_missiles
    }

# ------------------- 差分进化全局优化 -------------------
result = differential_evolution(
    obj,
    bounds,
    maxiter=1000,      # 增大迭代次数
    popsize=25,        # 增大种群规模
    polish=True,
    tol=1e-7,
    seed=42,
    disp=True
)

x_opt = result.x
best_cover = -result.fun

# ------------------- 输出结果 -------------------
print("\n无人机运动信息:")
for u in range(uav_count):
    speed_idx = (u * (smoke_per_uav * 2 + 2))
    speed = x_opt[speed_idx]
    angle = x_opt[speed_idx + 1]
    print(f"UAV{u+1}: 速度 = {speed:.2f} m/s, 方向 = {np.degrees(angle):.2f}°")

print("\n烟幕弹详细信息:")
rows = []
for u in range(uav_count):
    for k in range(smoke_per_uav):
        smoke_info = calculate_smoke_info(x_opt, u, k)
        
        release_pos = smoke_info['release_pos']
        blast_pos = smoke_info['blast_pos']
        
        print(f"\nUAV{u+1} 烟幕弹{k+1}:")
        print(f"  投放时间: {x_opt[(u * (smoke_per_uav * 2 + 2)) + 2 + (k * 2)]:.2f} s")
        print(f"  引信时间: {x_opt[(u * (smoke_per_uav * 2 + 2)) + 2 + (k * 2) + 1]:.2f} s")
        print(f"  投放坐标: ({release_pos[0]:.2f}, {release_pos[1]:.2f}, {release_pos[2]:.2f}) m")
        print(f"  爆炸坐标: ({blast_pos[0]:.2f}, {blast_pos[1]:.2f}, {blast_pos[2]:.2f}) m")
        print(f"  单个遮蔽时长: {smoke_info['cover_time']:.2f} s")
        print(f"  干扰的导弹: {', '.join(map(str, smoke_info['affected_missiles'])) or '无'}")
        
        rows.append([
            f"UAV{u+1}", 
            k+1, 
            x_opt[(u * (smoke_per_uav * 2 + 2)) + 2 + (k * 2)],
            x_opt[(u * (smoke_per_uav * 2 + 2)) + 2 + (k * 2) + 1],
            x_opt[(u * (smoke_per_uav * 2 + 2)) + 2 + (k * 2)] + x_opt[(u * (smoke_per_uav * 2 + 2)) + 2 + (k * 2) + 1],
            x_opt[(u * (smoke_per_uav * 2 + 2)) + 2 + (k * 2)] + x_opt[(u * (smoke_per_uav * 2 + 2)) + 2 + (k * 2) + 1] + smoke_burn,
            smoke_info['speed'],
            smoke_info['angle_deg'],
            f"({release_pos[0]:.2f}, {release_pos[1]:.2f}, {release_pos[2]:.2f})",
            f"({blast_pos[0]:.2f}, {blast_pos[1]:.2f}, {blast_pos[2]:.2f})",
            smoke_info['cover_time'],
            ', '.join(map(str, smoke_info['affected_missiles'])) or '无'
        ])

# 使用中文表头
df = pd.DataFrame(rows, columns=[
    "无人机", "烟幕弹序号", "投放时间(s)", "引信时间(s)", "起爆时刻(s)", "结束时刻(s)", 
    "速度(m/s)", "角度(度)", "投放坐标", "爆炸坐标",
    "遮蔽时长(s)", "干扰导弹编号"
])
df = df.sort_values(by="起爆时刻(s)").reset_index(drop=True)
print("\n汇总表格:")
print(df.to_string(index=False))
print(f"\n>>> 总遮蔽时长 (3枚导弹): {best_cover:.2f} s")

# 保存文件，使用中文表头和UTF-8编码
df.to_csv("result5.csv", index=False, encoding='utf-8-sig')
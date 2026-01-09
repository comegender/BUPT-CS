import numpy as np
import math
import matplotlib.pyplot as plt
import sys
import sys

def uprint(*args, sep=' ', end='\n'):
    # 拼接字符串，编码成 UTF-8，再写入 stdout buffer
    s = sep.join(map(str,args)) + end
    sys.stdout.buffer.write(s.encode('utf-8'))

# ---------- 常量与输入（按题面给出） ----------
g = 9.8                    # 重力加速度 m/s^2
R0 = 10.0                  # 烟幕有效半径 m
sink_v = 3.0               # 云团匀速下沉速度 m/s
v_missile = 300.0          # 导弹速度 m/s
v_uav = 120.0              # FY1 飞行速度 m/s (题中已指定)
t_release = 1.5            # 投放延时 s
t_fuze = 3.6               # 起爆延时 s
t_blast = t_release + t_fuze
active_window = 20.0       # 云团有效时间窗 s

# 初始位置（题面）
M0 = np.array([20000.0, 0.0, 2000.0])   # M1 初始
FY0 = np.array([17800.0, 0.0, 1800.0])  # FY1 初始
T = np.array([0.0, 200.0, 5.0])         # 真目标代表点（用圆柱中心近似）

# ---------- 工具函数 ----------
def unit(v):
    n = np.linalg.norm(v)
    return v / n if n != 0 else v

# ---------- 轨迹与投放计算 ----------
# 导弹方向（指向原点）
uM = -unit(M0)                # 单位方向向量（导弹向原点飞）
# 无人机航向：水平朝原点（等高度直线飞）
vec_xy = np.array([0.0, 0.0]) - FY0[:2]
uav_dir_xy = vec_xy / np.linalg.norm(vec_xy)
uav_dir = np.array([uav_dir_xy[0], uav_dir_xy[1], 0.0])  # z=0（等高度）

# 投放点（t = t_release）
FY_release = FY0 + v_uav * t_release * uav_dir

# 起爆前的运动（投放到起爆时间段 dt = t_fuze）
v0 = v_uav * uav_dir          # 投放时的初速度（水平）
dt = t_fuze
C0 = FY_release + v0 * dt + np.array([0.0, 0.0, -0.5 * g * dt * dt])  # 起爆时云团中心

# 导弹位置函数
def missile_pos(t):
    return M0 + v_missile * t * uM

# 云团中心随时间（起爆后匀速下沉）：
def cloud_center(t):
    if t < t_blast:
        return C0  # technically not formed before t_blast, but keep C0 for continuity
    return C0 + np.array([0.0, 0.0, -sink_v * (t - t_blast)])

# 最近点到线段距离（返回 d, s, closest_point）
def dist_point_to_segment(P, A, B):
    AB = B - A
    AP = P - A
    ab2 = np.dot(AB, AB)
    if ab2 == 0.0:
        return np.linalg.norm(P - A), 0.0, A
    s = np.dot(AP, AB) / ab2
    s_clamped = max(0.0, min(1.0, s))
    closest = A + s_clamped * AB
    d = np.linalg.norm(P - closest)
    return d, s, closest

# ---------- 扫描与精确化边界 ----------
t_start_scan = t_blast
t_end_scan = t_blast + active_window
dt_scan = 0.001  # 1 ms 步长，足够精细
t_vals = np.arange(t_start_scan, t_end_scan + 1e-12, dt_scan)

covered_bool = []
for t in t_vals:
    M = missile_pos(t)
    C = cloud_center(t)
    d, s, _ = dist_point_to_segment(C, M, T)
    covered_bool.append((d <= R0) and (s > 0.0) and (s < 1.0))

# 找出连续覆盖区间（粗略）
intervals = []
in_cov = False
for i,flag in enumerate(covered_bool):
    if flag and not in_cov:
        start = t_vals[i]
        in_cov = True
    if in_cov and (not flag):
        end = t_vals[i-1]
        intervals.append((start, end))
        in_cov = False
if in_cov:
    intervals.append((start, t_vals[-1]))

# 对每个粗区间做二分法精确边界（针对 d=R0 与 s=0 或 s=1 的变化）
def refine_boundary(func, a, b, tol=1e-10, maxit=80):
    fa = func(a); fb = func(b)
    if fa * fb > 0:
        return None
    for _ in range(maxit):
        m = 0.5*(a+b)
        fm = func(m)
        if abs(fm) < tol or (b-a) < 1e-12:
            return m
        if fa * fm <= 0:
            b, fb = m, fm
        else:
            a, fa = m, fm
    return 0.5*(a+b)

refined_intervals = []
for (a_idx,b_idx) in [(int(a/dt_scan), int(b/dt_scan)) for (a,b) in intervals]:
    a = t_vals[a_idx]; b = t_vals[b_idx]
    # left boundary: solve d(t)-R0 = 0 within [a-dt_scan, a+dt_scan]
    def f_d(t):
        M = missile_pos(t); C = cloud_center(t)
        d, s, _ = dist_point_to_segment(C, M, T)
        return d - R0
    left = refine_boundary(f_d, max(t_start_scan, a-dt_scan), a+dt_scan) or a
    # right boundary: solve s(t)=0 (closest point reaches missile) or d(t)-R0=0 near b
    def f_s(t):
        M = missile_pos(t); C = cloud_center(t)
        d, s, _ = dist_point_to_segment(C, M, T)
        return s
    # choose bracket near b
    right = refine_boundary(f_s, b-dt_scan, min(t_end_scan, b+dt_scan))
    if right is None:
        right = refine_boundary(f_d, b-dt_scan, min(t_end_scan, b+dt_scan)) or b
    refined_intervals.append((left, right))

# 合并并计算总时长
merged = []
for seg in refined_intervals:
    if not merged:
        merged.append(seg)
    else:
        if seg[0] <= merged[-1][1] + 1e-9:
            merged[-1] = (merged[-1][0], max(merged[-1][1], seg[1]))
        else:
            merged.append(seg)

total_time = sum(e - s for (s,e) in merged)

# ---------- 输出关键数据 ----------
uprint("===== 关键数值 =====")
uprint("投放时刻 t_release = {:.3f} s".format(t_release))
uprint("起爆时刻 t_blast  = {:.3f} s".format(t_blast))
uprint("FY1 投放点 (t=1.5s) = [{:.3f}, {:.3f}, {:.3f}]".format(*FY_release))
uprint("云团起爆点 C0 = [{:.3f}, {:.3f}, {:.3f}]".format(*C0))
uprint("导弹位置 M(t_blast) = [{:.3f}, {:.3f}, {:.3f}]".format(*missile_pos(t_blast)))
uprint()
uprint("检测到的覆盖（遮蔽）区间（精确化后）：")
for s,e in merged:
    uprint("  -> [{:.9f} s, {:.9f} s], duration = {:.6f} s".format(s,e,e-s))
uprint("总遮蔽时长 = {:.6f} s".format(total_time))

import numpy as np
import math
from scipy.optimize import differential_evolution

# ---------- 常量与输入 ----------
g = 9.8
R0 = 10.0
sink_v = 3.0
v_missile = 300.0
FY0 = np.array([17800.0, 0.0, 1800.0])
M0 = np.array([20000.0, 0.0, 2000.0])
T = np.array([0.0, 200.0, 5.0])
active_window = 20.0
base_angle = math.atan2(-FY0[1], -FY0[0])
uM = -(M0 / np.linalg.norm(M0))

def missile_pos(t):
    return M0 + v_missile * t * uM

def dist_point_to_segment(P, A, B):
    AB = B - A
    AP = P - A
    ab2 = np.dot(AB, AB)
    if ab2 == 0.0:
        return np.linalg.norm(P - A), 0.0, A
    s = np.dot(AP, AB) / ab2
    s_clamped = max(0.0, min(1.0, s))
    closest = A + s_clamped * AB
    return np.linalg.norm(P - closest), s, closest

def compute_cover_time(theta_offset, v_uav, t_release, t_fuze, dt=0.01):
    heading = base_angle + theta_offset
    uav_dir = np.array([math.cos(heading), math.sin(heading), 0.0])
    FY_release = FY0 + v_uav * t_release * uav_dir
    v0 = v_uav * uav_dir
    C0 = FY_release + v0 * t_fuze + np.array([0,0,-0.5*g*t_fuze*t_fuze])
    def cloud_center(t):
        if t < t_release + t_fuze:
            return C0
        return C0 + np.array([0,0,-sink_v*(t - (t_release + t_fuze))])
    t_start = t_release + t_fuze
    t_end = t_start + active_window
    t_vals = np.arange(t_start, t_end+1e-12, dt)
    covered = []
    for t in t_vals:
        d, s, _ = dist_point_to_segment(cloud_center(t), missile_pos(t), T)
        covered.append((d <= R0) and (0 < s < 1))
    total = 0.0
    in_cov = False
    t0 = 0.0
    for i, flag in enumerate(covered):
        if flag and not in_cov:
            t0 = t_vals[i]; in_cov = True
        if in_cov and (not flag):
            total += t_vals[i-1] - t0
            in_cov = False
    if in_cov:
        total += t_vals[-1] - t0
    return total

# 优化目标
def objective(x):
    theta, v, trel, tfuz = x
    # 限制搜索在合理范围
    if v < 70 or v > 140 or trel < 0 or tfuz < 0:
        return 1e6
    return -compute_cover_time(theta, v, trel, tfuz, dt=0.02)

# 注意：收紧搜索范围，避免搜索无效解
bounds = [
    (-math.pi/2, math.pi/2),  # 航向偏移：只搜 ±90°，防止乱飞
    (70, 140),                # 速度
    (0, 3),                   # 投放延时
    (0.5, 5)                  # 引信延时
]

result = differential_evolution(objective, bounds, maxiter=50, popsize=25, polish=True, tol=1e-6, seed=42)
theta_opt, v_opt, trel_opt, tfuz_opt = result.x
final_cover = compute_cover_time(theta_opt, v_opt, trel_opt, tfuz_opt, dt=0.001)

print("=== 优化结果 ===")
print(f"航向偏移 theta_offset = {theta_opt:.6f} rad  ({math.degrees(theta_opt):.3f}°)")
print(f"无人机速度 v = {v_opt:.3f} m/s")
print(f"投放延时 t_release = {trel_opt:.3f} s")
print(f"引信延时 t_fuze = {tfuz_opt:.3f} s")
print(f"最大遮蔽时长 = {final_cover:.3f} s")
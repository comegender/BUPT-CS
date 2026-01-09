import numpy as np
import math
import sys
from scipy.optimize import differential_evolution

def uprint(*args, sep=' ', end='\n'):
    """确保UTF-8编码的中文能正确打印。"""
    s = sep.join(map(str, args)) + end
    sys.stdout.buffer.write(s.encode('utf-8'))

# ---------- 常量与输入 ----------
G = 9.8
R_SMOKE = 10.0
V_SINK = 3.0
V_MISSILE = 300.0
T_SMOKE_EFFECTIVE = 20.0

# 初始位置
P_UAVS_INITIAL = {
    'FY1': np.array([17800.0, 0.0, 1800.0]),
    'FY2': np.array([12000.0, 1400.0, 1400.0]),
    'FY3': np.array([6000.0, -3000.0, 700.0])
}
P_M1 = np.array([20000.0, 0.0, 2000.0])
P_TARGET_REAL = np.array([0.0, 200.0, 5.0])
P_TARGET_FAKE = np.array([0.0, 0.0, 0.0])

# 预计算的常量向量
MISSILE_DIR = (P_TARGET_FAKE - P_M1) / np.linalg.norm(P_TARGET_FAKE - P_M1)
BASE_ANGLES = {
    name: math.atan2(P_TARGET_FAKE[1] - pos[1], P_TARGET_FAKE[0] - pos[0])
    for name, pos in P_UAVS_INITIAL.items()
}
UAV_NAMES = list(P_UAVS_INITIAL.keys())

# ---------- 工具函数 ----------
def missile_pos(t):
    return P_M1 + V_MISSILE * t * MISSILE_DIR

def dist_point_to_segment(P, A, B):
    AB = B - A
    AP = P - A
    ab2 = np.dot(AB, AB)
    if ab2 == 0.0: return np.linalg.norm(P - A), 0.0
    s = np.dot(AP, AB) / ab2
    return np.linalg.norm(P - (A + s * AB)), s

# ---------- 核心计算函数 ----------
def compute_cover_time_multi_uavs(x, dt=0.02):
    """
    计算多无人机、各1枚弹的总遮蔽时长（并集）。
    x: [theta1, v1, tr1, tf1, theta2, v2, tr2, tf2, ...] (12个变量)
    """
    blast_params = []
    num_uavs = len(x) // 4
    for i in range(num_uavs):
        uav_name = UAV_NAMES[i]
        params = x[i*4 : (i+1)*4]
        theta_offset, v_uav, t_rel, t_fuz = params

        heading = BASE_ANGLES[uav_name] + theta_offset
        uav_dir = np.array([math.cos(heading), math.sin(heading), 0.0])
        v_uav_vec = v_uav * uav_dir
        
        p_release = P_UAVS_INITIAL[uav_name] + v_uav_vec * t_rel
        p_blast = p_release + v_uav_vec * t_fuz + np.array([0, 0, -0.5 * G * t_fuz**2])
        t_blast = t_rel + t_fuz
        blast_params.append({'p_blast': p_blast, 't_blast': t_blast})

    # --- 模拟与扫描 ---
    if not blast_params: return 0.0
    t_start_scan = min(p['t_blast'] for p in blast_params)
    t_end_scan = max(p['t_blast'] for p in blast_params) + T_SMOKE_EFFECTIVE
    t_vals = np.arange(t_start_scan, t_end_scan, dt)
    
    total_time = 0
    for t in t_vals:
        p_missile_t = missile_pos(t)
        is_covered_at_t = False
        for params in blast_params:
            t_after_blast = t - params['t_blast']
            if 0 <= t_after_blast < T_SMOKE_EFFECTIVE:
                p_cloud_center = params['p_blast'] + np.array([0, 0, -V_SINK * t_after_blast])
                d, s = dist_point_to_segment(p_cloud_center, p_missile_t, P_TARGET_REAL)
                if d < R_SMOKE and 0 < s < 1:
                    is_covered_at_t = True
                    break
        if is_covered_at_t:
            total_time += dt
            
    return total_time

def compute_single_cover_time(uav_name, single_x, dt=0.01):
    """
    计算单架无人机单枚弹的独立遮蔽时长。
    uav_name: 'FY1', 'FY2', or 'FY3'
    single_x: [theta_offset, v_uav, t_release, t_fuze]
    """
    theta_offset, v_uav, t_release, t_fuze = single_x

    heading = BASE_ANGLES[uav_name] + theta_offset
    uav_dir = np.array([math.cos(heading), math.sin(heading), 0.0])
    v_uav_vec = v_uav * uav_dir
    
    p_release = P_UAVS_INITIAL[uav_name] + v_uav_vec * t_release
    p_blast = p_release + v_uav_vec * t_fuze + np.array([0, 0, -0.5 * G * t_fuze**2])
    t_blast = t_release + t_fuze

    t_start_scan = t_blast
    t_end_scan = t_blast + T_SMOKE_EFFECTIVE
    t_vals = np.arange(t_start_scan, t_end_scan, dt)
    
    cover_time = 0
    for t in t_vals:
        p_missile_t = missile_pos(t)
        t_after_blast = t - t_blast
        p_cloud_center = p_blast + np.array([0, 0, -V_SINK * t_after_blast])
        d, s = dist_point_to_segment(p_cloud_center, p_missile_t, P_TARGET_REAL)
        if d < R_SMOKE and 0 < s < 1:
            cover_time += dt
            
    return cover_time

# ---------- 优化目标函数 ----------
def objective(x):
    return -compute_cover_time_multi_uavs(x, dt=0.1)

# ---------- 主程序：差分进化优化 ----------
def solve_problem4():
    uprint("--- 问题4：三无人机各一弹最优策略求解 ---")
    uprint("正在使用差分进化算法进行全局优化，变量维度较高，预计需要很长时间...")

    # 决策变量边界: 3 * 4 = 12个变量
    # [theta1, v1, tr1, tf1, theta2, v2, tr2, tf2, theta3, v3, tr3, tf3]
    bounds = []
    for name in UAV_NAMES:
        # 航向偏移, 速度, 投放延时, 引信延时
        uav_bounds = [
            (-math.pi / 3, math.pi / 3), # 航向偏移: ±60°
            (100, 140),                  # 速度 (m/s)
            (1.0, 25.0),                 # 投放延时 (s)
            (1.0, 20.0),                 # 引信延时 (s)
        ]
        bounds.extend(uav_bounds)

    # 调用差分进化求解器
    result = differential_evolution(
        objective, 
        bounds, 
        maxiter=500,      # 针对高维度问题，增加迭代次数
        popsize=20,       # 种群大小
        polish=True, 
        tol=1e-5,
        updating='deferred',
        workers=-1,
        seed=42
    )

    uprint("\n优化完成！")
    
    # --- 使用高精度dt计算最终结果 ---
    final_cover_time = compute_cover_time_multi_uavs(result.x, dt=0.005)
    
    # --- 结果输出 ---
    uprint("\n" + "="*20 + " 最优策略 " + "="*20)
    
    total_individual_time = 0
    for i in range(len(UAV_NAMES)):
        uav_name = UAV_NAMES[i]
        params = result.x[i*4 : (i+1)*4]
        theta_opt, v_opt, tr_opt, tf_opt = params
        
        # 计算单弹独立贡献
        single_cover = compute_single_cover_time(uav_name, params, dt=0.005)
        total_individual_time += single_cover

        # 计算投放坐标和爆炸坐标
        heading = BASE_ANGLES[uav_name] + theta_opt
        uav_dir = np.array([math.cos(heading), math.sin(heading), 0.0])
        v_uav_vec = v_opt * uav_dir
        
        # 投放坐标 = 初始位置 + 速度向量 * 投放时间
        p_release = P_UAVS_INITIAL[uav_name] + v_uav_vec * tr_opt
        
        # 爆炸坐标 = 投放坐标 + 速度向量 * 引信时间 + 自由落体位移
        p_blast = p_release + v_uav_vec * tf_opt + np.array([0, 0, -0.5 * G * tf_opt**2])

        uprint(f"\n--- 无人机 {uav_name} 策略 ---")
        uprint(f"  飞行速度: {v_opt:.4f} m/s")
        uprint(f"  飞行航向: {np.rad2deg(BASE_ANGLES[uav_name] + theta_opt):.4f} 度 (相对基准偏移 {np.rad2deg(theta_opt):.4f} 度)")
        uprint(f"  投放时间: {tr_opt:.4f} s")
        uprint(f"  引信时间: {tf_opt:.4f} s")
        uprint(f"  起爆时刻: {tr_opt + tf_opt:.4f} s")
        uprint(f"  投放坐标: ({p_release[0]:.2f}, {p_release[1]:.2f}, {p_release[2]:.2f}) m")
        uprint(f"  爆炸坐标: ({p_blast[0]:.2f}, {p_blast[1]:.2f}, {p_blast[2]:.2f}) m")
        uprint(f"  [独立遮蔽贡献: {single_cover:.4f} s]")

    uprint(f"\n注意: 各弹独立遮蔽时长之和 ({total_individual_time:.4f} s) 可能因时间重叠而大于总有效遮蔽时长。")
    uprint("\n" + "-"*50)
    uprint(f"找到的总有效遮蔽时长（并集）为: {final_cover_time:.4f} 秒")
    uprint("-"*50)

if __name__ == '__main__':
    solve_problem4()
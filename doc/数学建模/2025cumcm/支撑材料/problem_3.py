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
P_UAV1_INITIAL = np.array([17800.0, 0.0, 1800.0])
P_M1 = np.array([20000.0, 0.0, 2000.0])
P_TARGET_REAL = np.array([0.0, 200.0, 5.0])
P_TARGET_FAKE = np.array([0.0, 0.0, 0.0])

# 预计算的常量向量
MISSILE_DIR = (P_TARGET_FAKE - P_M1) / np.linalg.norm(P_TARGET_FAKE - P_M1)
BASE_ANGLE = math.atan2(P_TARGET_FAKE[1] - P_UAV1_INITIAL[1], P_TARGET_FAKE[0] - P_UAV1_INITIAL[0])

# ---------- 工具函数 ----------
def missile_pos(t):
    return P_M1 + V_MISSILE * t * MISSILE_DIR

def dist_point_to_segment(P, A, B):
    AB = B - A
    AP = P - A
    ab2 = np.dot(AB, AB)
    if ab2 == 0.0: return np.linalg.norm(P - A), 0.0
    s = np.dot(AP, AB) / ab2
    # s_clamped = max(0.0, min(1.0, s)) # 在判断逻辑中处理s的范围
    closest = A + s * AB
    return np.linalg.norm(P - closest), s

def calculate_release_and_blast_positions(theta_offset, v_uav, t_rel, t_fuz):
    """计算投放点和爆炸点坐标"""
    heading = BASE_ANGLE + theta_offset
    uav_dir = np.array([math.cos(heading), math.sin(heading), 0.0])
    v_uav_vec = v_uav * uav_dir
    
    # 投放点坐标
    p_release = P_UAV1_INITIAL + v_uav_vec * t_rel
    
    # 爆炸点坐标 (考虑自由落体)
    p_blast = p_release + v_uav_vec * t_fuz + np.array([0, 0, -0.5 * G * t_fuz**2])
    
    return p_release, p_blast

# ---------- 核心计算函数 (3枚弹) ----------
def compute_cover_time_3_grenades(x, dt=0.02):
    """
    计算3枚弹的总遮蔽时长（并集）。
    x: [theta_offset, v_uav, t_rel1, t_fuz1, dt_rel2, t_fuz2, dt_rel3, t_fuz3]
    """
    theta_offset, v_uav, t_rel1, t_fuz1, dt_rel2, t_fuz2, dt_rel3, t_fuz3 = x

    # --- 计算弹道与起爆点 ---
    heading = BASE_ANGLE + theta_offset
    uav_dir = np.array([math.cos(heading), math.sin(heading), 0.0])
    v_uav_vec = v_uav * uav_dir

    # 计算真实的投放时间
    t_rel2 = t_rel1 + dt_rel2
    t_rel3 = t_rel2 + dt_rel3
    
    release_times = [t_rel1, t_rel2, t_rel3]
    fuze_times = [t_fuz1, t_fuz2, t_fuz3]
    
    blast_params = []
    for t_rel, t_fuz in zip(release_times, fuze_times):
        p_release = P_UAV1_INITIAL + v_uav_vec * t_rel
        p_blast = p_release + v_uav_vec * t_fuz + np.array([0, 0, -0.5 * G * t_fuz**2])
        t_blast = t_rel + t_fuz
        blast_params.append({'p_blast': p_blast, 't_blast': t_blast})

    # --- 模拟与扫描 ---
    # 确定仿真时间范围
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
                    break # 当前时间点已被覆盖，无需检查其他弹
        
        if is_covered_at_t:
            total_time += dt
            
    return total_time

def compute_single_grenade_cover_time(x_single, dt=0.01):
    """
    计算单枚弹的独立遮蔽时长。
    x_single: [theta_offset, v_uav, t_release, t_fuze]
    """
    theta_offset, v_uav, t_release, t_fuze = x_single

    # --- 计算弹道与起爆点 ---
    heading = BASE_ANGLE + theta_offset
    uav_dir = np.array([math.cos(heading), math.sin(heading), 0.0])
    v_uav_vec = v_uav * uav_dir
    
    p_release = P_UAV1_INITIAL + v_uav_vec * t_release
    p_blast = p_release + v_uav_vec * t_fuze + np.array([0, 0, -0.5 * G * t_fuze**2])
    t_blast = t_release + t_fuze

    # --- 模拟与扫描 ---
    t_start_scan = t_blast
    t_end_scan = t_blast + T_SMOKE_EFFECTIVE
    t_vals = np.arange(t_start_scan, t_end_scan, dt)
    
    single_cover_time = 0
    for t in t_vals:
        p_missile_t = missile_pos(t)
        p_cloud_center = p_blast + np.array([0, 0, -V_SINK * (t - t_blast)])
        d, s = dist_point_to_segment(p_cloud_center, p_missile_t, P_TARGET_REAL)
        if d < R_SMOKE and 0 < s < 1:
            single_cover_time += dt
            
    return single_cover_time

# ---------- 优化目标函数 ----------
def objective(x):
    """优化器调用的目标函数，返回负时长。"""
    # 使用粗糙的dt进行快速评估
    return -compute_cover_time_3_grenades(x, dt=0.1)

# ---------- 主程序：差分进化优化 ----------
def solve_problem3():
    uprint("--- 问题3：单无人机三弹最优策略求解 ---")
    uprint("正在使用差分进化算法进行全局优化，这可能需要较长时间...")

    # 决策变量边界: 8个变量
    # 借鉴A3.py的思路，收紧边界以提高效率
    # [theta_offset, v_uav, t_rel1, t_fuz1, dt_rel2, t_fuz2, dt_rel3, t_fuz3]
    bounds = [
        (-math.pi / 4, math.pi / 4), # 航向偏移: 进一步收紧至 ±45°
        (100, 140),                  # 速度 (m/s): 高速通常更有利于快速部署
        (0.5, 5.0),                  # 第1枚弹投放延时 (s)
        (1.0, 8.0),                  # 第1枚弹引信延时 (s)
        (1.0, 15.0),                 # 第2枚弹投放间隔 (s), 下限为1s
        (1.0, 8.0),                  # 第2枚弹引信延时 (s)
        (1.0, 15.0),                 # 第3枚弹投放间隔 (s), 下限为1s
        (1.0, 8.0),                  # 第3枚弹引信延时 (s)
    ]

    # 调用差分进化求解器
    result = differential_evolution(
        objective, 
        bounds, 
        maxiter=300,      # 增加迭代次数以进行更充分的搜索
        popsize=25,       # 增加种群大小
        polish=True, 
        tol=1e-6,
        updating='deferred', # 并行计算
        workers=-1,          # 使用所有CPU核心
        seed=42
    )

    uprint("\n优化完成！")
    
    # --- 使用高精度dt计算最终结果 ---
    final_cover_time = compute_cover_time_3_grenades(result.x, dt=0.005)
    
    # --- 结果输出 ---
    uprint("\n" + "="*20 + " 最优策略 " + "="*20)
    
    theta_opt, v_opt, tr1_opt, tf1_opt, dtr2_opt, tf2_opt, dtr3_opt, tf3_opt = result.x
    tr2_opt = tr1_opt + dtr2_opt
    tr3_opt = tr2_opt + dtr3_opt

    # 计算每枚弹的独立贡献时长
    high_precision_dt = 0.005
    cover1 = compute_single_grenade_cover_time([theta_opt, v_opt, tr1_opt, tf1_opt], dt=high_precision_dt)
    cover2 = compute_single_grenade_cover_time([theta_opt, v_opt, tr2_opt, tf2_opt], dt=high_precision_dt)
    cover3 = compute_single_grenade_cover_time([theta_opt, v_opt, tr3_opt, tf3_opt], dt=high_precision_dt)

    # 计算投放点和爆炸点坐标
    p_rel1, p_blast1 = calculate_release_and_blast_positions(theta_opt, v_opt, tr1_opt, tf1_opt)
    p_rel2, p_blast2 = calculate_release_and_blast_positions(theta_opt, v_opt, tr2_opt, tf2_opt)
    p_rel3, p_blast3 = calculate_release_and_blast_positions(theta_opt, v_opt, tr3_opt, tf3_opt)

    uprint(f"无人机飞行速度: {v_opt:.4f} m/s")
    uprint(f"无人机飞行航向: {np.rad2deg(BASE_ANGLE + theta_opt):.4f} 度 (相对基准偏移 {np.rad2deg(theta_opt):.4f} 度)")
    uprint("\n--- 干扰弹投放详情 ---")
    
    # 弹1信息
    uprint(f"弹1:")
    uprint(f"  投放时间 = {tr1_opt:.4f} s")
    uprint(f"  投放点坐标 = ({p_rel1[0]:.2f}, {p_rel1[1]:.2f}, {p_rel1[2]:.2f})")
    uprint(f"  引信时间 = {tf1_opt:.4f} s")
    uprint(f"  爆炸点坐标 = ({p_blast1[0]:.2f}, {p_blast1[1]:.2f}, {p_blast1[2]:.2f})")
    uprint(f"  起爆时刻 = {tr1_opt + tf1_opt:.4f} s")
    uprint(f"  独立遮蔽时长 = {cover1:.4f} s")
    
    # 弹2信息
    uprint(f"\n弹2:")
    uprint(f"  投放时间 = {tr2_opt:.4f} s")
    uprint(f"  投放点坐标 = ({p_rel2[0]:.2f}, {p_rel2[1]:.2f}, {p_rel2[2]:.2f})")
    uprint(f"  引信时间 = {tf2_opt:.4f} s")
    uprint(f"  爆炸点坐标 = ({p_blast2[0]:.2f}, {p_blast2[1]:.2f}, {p_blast2[2]:.2f})")
    uprint(f"  起爆时刻 = {tr2_opt + tf2_opt:.4f} s")
    uprint(f"  独立遮蔽时长 = {cover2:.4f} s")
    
    # 弹3信息
    uprint(f"\n弹3:")
    uprint(f"  投放时间 = {tr3_opt:.4f} s")
    uprint(f"  投放点坐标 = ({p_rel3[0]:.2f}, {p_rel3[1]:.2f}, {p_rel3[2]:.2f})")
    uprint(f"  引信时间 = {tf3_opt:.4f} s")
    uprint(f"  爆炸点坐标 = ({p_blast3[0]:.2f}, {p_blast3[1]:.2f}, {p_blast3[2]:.2f})")
    uprint(f"  起爆时刻 = {tr3_opt + tf3_opt:.4f} s")
    uprint(f"  独立遮蔽时长 = {cover3:.4f} s")
    
    uprint(f"\n注意: 各弹独立遮蔽时长之和 ({cover1+cover2+cover3:.4f} s) 可能因时间重叠而大于总有效遮蔽时长。")
    uprint("\n" + "-"*50)
    uprint(f"找到的总有效遮蔽时长为: {final_cover_time:.4f} 秒")
    uprint("-"*50)

if __name__ == '__main__':
    solve_problem3()
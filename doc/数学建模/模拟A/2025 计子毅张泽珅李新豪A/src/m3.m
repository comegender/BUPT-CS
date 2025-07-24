% m3.m - 变高度蒙特卡洛优化（修正版）
clear;

%% 1. 加载优化结果（修正加载方式）
try
    load('optimal_D.mat');    % 吸收塔南移距离
    load('optimal_WH.mat');   % 定日镜尺寸 W,H
    load('optimal_dr.mat');   % 定日镜间距
catch ME
    error('文件加载失败: %s\n请先运行m21.m, m22.m, m23.m', ME.message);
end

%% 2. 参数初始化
n_tiers = length(100:optimal_dr:(350+optimal_D));  % 计算圈数
max_iter = 1e6;
best_Ef = 0;

% 生成极坐标布局（复用m21逻辑）
x = []; y = [];
R = 100:optimal_dr:(350+optimal_D);
for i = 1:length(R)
    beta = optimal_dr/R(i);
    theta = ((-1)^i*beta/4 + asin((R(i)^2+optimal_D^2-350^2)/(2*R(i)*optimal_D)) + beta/2) : beta : ...
           (pi - asin((R(i)^2+optimal_D^2-350^2)/(2*R(i)*optimal_D)) - beta/4);
    x = [x, R(i)*cos(theta)];
    y = [y, R(i)*sin(theta)];
end
location = [x', y'-optimal_D];

%% 3. 蒙特卡洛主循环
progress = waitbar(0,'优化进行中...');
for iter = 1:max_iter
    % 随机生成各圈高度 (h_tj ∈ [W/2, 6])
    h_tj = optimal_WH(1)/2 + (6-optimal_WH(1)/2)*rand(1,n_tiers);
    
    % 分配高度到每个定日镜
    h_arr = zeros(size(location,1),1);
    tier_idx = discretize(sqrt(location(:,1).^2 + (location(:,2)+optimal_D).^2),...
                        [100:optimal_dr:(350+optimal_D), inf]);
    
    for k = 1:n_tiers
        h_arr(tier_idx==k) = h_tj(k);
    end
    
    % 计算目标函数
    [year_Ef, ~] = Objfun3(optimal_WH(2), optimal_WH(1), h_arr, optimal_D, location);
    
    % 更新最优解
    if year_Ef > best_Ef
        best_Ef = year_Ef;
        best_h = h_arr;
        save('best_height.mat', 'best_h', 'best_Ef');
        waitbar(iter/max_iter, progress, sprintf('当前最优: %.2f MW', best_Ef));
    end
end
close(progress);
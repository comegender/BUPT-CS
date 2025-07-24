% m23.m - 定日镜间距优化（单峰搜索）
clear;
load('optimal_D.mat', 'optimal_D');
load('optimal_WH.mat', 'optimal_WH');

%% 参数初始化
D = optimal_D;
W = optimal_WH(1); H = optimal_WH(2);
h = 6;
dr_range = (W+5-1):0.2:(W+5+1);  % 在基准间距附近搜索

%% 间距敏感性分析
results = [];
for dr = dr_range
    %% 定日镜场布局
    R = 100:dr:(350+D);
    x = []; y = [];
    for i = 1:length(R)
        beta = dr/R(i);
        theta = ((-1)^i*beta/4 + asin((R(i)^2+D^2-350^2)/(2*R(i)*D)) + beta/2) : beta : (pi - asin((R(i)^2+D^2-350^2)/(2*R(i)*D)) - beta/4);
        x = [x, R(i)*cos(theta)];
        y = [y, R(i)*sin(theta)];
    end
    
    %% 计算目标函数
    location = [x', y'-D];
    [year_EF, ~] = Objfun2(H, W, h, D, location);
    results = [results; dr, year_EF];
    
    % 提前终止条件
    if size(results,1)>2 && diff(results(end-1:end,2))<0 && diff(results(end-2:end-1,2))<0
        break
    end
end

%% 结果保存
optimal_dr = results(end,1);
save('optimal_dr.mat', 'optimal_dr');
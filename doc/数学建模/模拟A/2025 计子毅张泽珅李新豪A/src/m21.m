% m21.m - 吸收塔位置优化（三分查找法）
clear;
load('s_in.mat'); load('alphas.mat');

%% 参数初始化
W = 8; H = 8; h = 6;  % 定日镜固定参数
e = 1;  % 收敛阈值
mind = 0; maxd = 250;  % 吸收塔南移距离范围

%% 三分查找主循环
while (maxd - mind) > e
    leftd = mind + (maxd - mind)/3;
    rightd = mind + 2*(maxd - mind)/3;
    
    YEAR_EF = [];
    for D = [leftd, rightd]  % 评估左右分位点
        %% 定日镜场布局生成
        dr = W + 5;  % 最小间距
        R = 100:dr:(350+D);  % 径向分布
        
        % 计算每圈角度范围
        theta_bond = zeros(1,length(R));
        for i = 1:length(R)
            if R(i) < 350-D
                theta_bond(i) = -pi/2;  % 全角度布置
            else
                theta_bond(i) = asin((R(i)^2 + D^2 - 350^2)/(2*R(i)*D));
            end
        end
        
        % 极坐标布置定日镜
        x = []; y = [];
        for i = 1:length(R)
            beta = dr/R(i);  % 相邻镜夹角
            theta = ((-1)^i*beta/4 + theta_bond(i) + beta/2) : beta : (pi - theta_bond(i) - beta/4);
            x = [x, R(i)*cos(theta)];
            y = [y, R(i)*sin(theta)];
        end
        
        %% 计算年平均输出功率
        location = [x', y'-D];
        [year_EF, ~] = Objfun2(H, W, h, D, location);
        YEAR_EF = [YEAR_EF, year_EF];
    end
    
    % 更新搜索区间
    if YEAR_EF(1) < YEAR_EF(2)
        mind = leftd;
    else
        maxd = rightd;
    end
end

%% 结果保存
optimal_D = (mind + maxd)/2;
save('optimal_D.mat', 'optimal_D');
function [year_Ef, year_Ef_per_area] = Objfun2(H, W, h, D, location)
    %% 初始化参数
    load('s_in.mat'); load('alphas.mat');
    loc_jire = [0, -D, 86]; % 含南移修正
    loc_dingri = [location, h*ones(size(location,1),1)];

    %% 向量化反射计算
    s_reflect = loc_jire - loc_dingri;
    s_reflect = s_reflect ./ vecnorm(s_reflect,2,2);

    %% 预分配内存
    eta = zeros(12,5,size(location,1));
    DNI = zeros(12,5);

    %% 计算各月各时刻效率
    for i = 1:12
        for j = 1:5
            % 1. 法向量计算
            sun_vec = s_in(i,3*(j-1)+(1:3));
            n_dingri = sun_vec - s_reflect;
            n_dingri = n_dingri ./ vecnorm(n_dingri,2,2);
            
            % 2. 余弦效率（向量化修正）
            eta_cos = abs(sum(n_dingri .* sun_vec, 2));
            
            % 3. 大气透射率
            d_HR = vecnorm(loc_dingri - loc_jire,2,2);
            eta_at = 0.99321 - 0.0001176*d_HR + 1.97e-8*d_HR.^2;
            
            % 4. 综合效率
            eta(i,j,:) = eta_cos .* eta_at * 0.92 * 0.85;
            
            % 5. 计算DNI
            a = 0.4237 - 0.00821*(6-3)^2;
            b = 0.5055 + 0.00595*(6.5-3)^2;
            c = 0.2711 + 0.01858*(2.5-3)^2;
            DNI(i,j) = 1.366*(a + b*exp(-c/sin(alphas(i,j))));
        end
    end

    %% 输出功率计算
    A = W * H;
    Ef = DNI .* A .* squeeze(mean(eta,3));
    year_Ef = mean(Ef,'all')/1000; % MW

    %% 添加约束条件（惩罚函数法）
    target_power = 48; % 目标功率48MW
    power_error = abs(year_Ef - target_power); % 计算偏差
    penalty_weight = 100; % 惩罚权重（可根据优化效果调整）
    
    % 如果功率偏离48MW，则施加惩罚
    if power_error > 0
        year_Ef = year_Ef - penalty_weight * power_error^2; % 二次惩罚项
    end

    %% 单位面积输出功率（可选）
    year_Ef_per_area = year_Ef*1000 / (size(location,1)*A); % kW/m2
end
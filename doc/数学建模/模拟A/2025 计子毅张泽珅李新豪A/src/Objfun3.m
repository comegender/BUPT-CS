function [year_Ef, year_Ef_per_area] = Objfun3(H, W, h, D, location)
% OBJFUN3 变高度定日镜场功率计算
% 输入：
%   H - 镜面高度(m)
%   W - 镜面宽度(m)
%   h - 安装高度数组(N×1)
%   D - 吸收塔南移距离(m)
%   location - 定日镜坐标(N×2)
% 输出：
%   year_Ef - 年平均输出功率(MW)
%   year_Ef_per_area - 单位面积功率(kW/m²)

%% 加载太阳数据
load('s_in.mat'); 
load('alphas.mat');

%% 初始化参数
loc_jire = [0, -D, 86]; % 集热器位置
eta = zeros(12,5,size(location,1));

%% 计算各定日镜效率
for i = 1:12 % 月份
    for j = 1:5 % 时间点
        % 1. 计算法向量
        sun_vec = s_in(i,3*(j-1)+(1:3));
        s_reflect = loc_jire - [location, h];
        s_reflect = s_reflect ./ vecnorm(s_reflect,2,2);
        n_dingri = sun_vec - s_reflect;
        n_dingri = n_dingri ./ vecnorm(n_dingri,2,2);
        
        % 2. 计算各效率项
        eta_cos = abs(sum(n_dingri .* sun_vec, 2));
        d_HR = vecnorm([location, h] - loc_jire, 2, 2);
        eta_at = 0.99321 - 0.0001176*d_HR + 1.97e-8*d_HR.^2;
        eta(i,j,:) = eta_cos .* eta_at * 0.92 * 0.85; % 含反射率和截断效率
    end
end

%% 计算输出功率
a = 0.4237 - 0.00821*(6-3)^2;
b = 0.5055 + 0.00595*(6.5-3)^2;
c = 0.2711 + 0.01858*(2.5-3)^2;
DNI = 1.366*(a + b*exp(-c./sin(alphas)));

A = W * H; % 单镜面积
Ef = squeeze(mean(DNI .* A .* eta, [1 2])); % 年平均
year_Ef = mean(Ef)/1000; % MW
year_Ef_per_area = year_Ef*1000 / (size(location,1)*A); % kW/m²
end
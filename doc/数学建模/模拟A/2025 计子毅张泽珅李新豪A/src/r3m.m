% 加载所有优化结果
load('optimal_D.mat');      % 吸收塔南移距离 optimal_D
load('optimal_WH.mat');     % 定日镜尺寸 optimal_WH = [W, H]
load('optimal_dr.mat');     % 定日镜间距 optimal_dr
load('best_height.mat');    % 最佳高度分布 best_h

%% 1. 吸收塔坐标 (假设位于坐标系原点(0,0)，向南移动optimal_D米)
absorber_x = 0;
absorber_y = -optimal_D;

%% 2. 重新生成定日镜场布局（确保与优化时一致）
x = []; y = [];
R = 100:optimal_dr:(350+optimal_D);
for i = 1:length(R)
    beta = optimal_dr/R(i);
    theta = ((-1)^i*beta/4 + asin((R(i)^2+optimal_D^2-350^2)/(2*R(i)*optimal_D)) + beta/2) : beta : ...
           (pi - asin((R(i)^2+optimal_D^2-350^2)/(2*R(i)*optimal_D)) - beta/4);
    x = [x, R(i)*cos(theta)];
    y = [y, R(i)*sin(theta)];
end
location = [x', y'-optimal_D];  % 定日镜的(x,y)坐标（y已减去optimal_D）

%% 3. 创建表格数据
num_heliostats = size(location, 1);
table_data = cell(num_heliostats+1, 8);  % 表头+数据行

% 表头
table_data(1,:) = {'吸收塔x坐标 (m)', '吸收塔y坐标 (m)', '定日镜序号', ...
                  '定日镜宽度 (m)', '定日镜高度 (m)', '定日镜x坐标 (m)', ...
                  '定日镜y坐标 (m)', '定日镜z坐标 (m)'};

% 吸收塔数据（第2行）
table_data{2,1} = absorber_x;
table_data{2,2} = absorber_y;

% 定日镜数据（从第3行开始）
for i = 1:num_heliostats
    table_data{i+2,3} = i;                          % 序号
    table_data{i+2,4} = optimal_WH(1);              % 宽度W
    table_data{i+2,5} = optimal_WH(2);              % 高度H
    table_data{i+2,6} = location(i,1);              % x坐标
    table_data{i+2,7} = location(i,2);              % y坐标（已相对吸收塔）
    table_data{i+2,8} = best_h(i);                   % z坐标（优化后的高度）
end

%% 4. 保存为Excel文件
filename = 'optimized_heliostat_layout.xlsx';
writecell(table_data, filename);

fprintf('表格数据已保存至: %s\n', filename);
disp('--------------------------------------');
disp('前5行数据预览:');
disp(table_data(1:min(6,size(table_data,1)),:));
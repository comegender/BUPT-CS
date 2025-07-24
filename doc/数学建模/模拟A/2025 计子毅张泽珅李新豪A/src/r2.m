% 加载优化结果
load('optimal_D.mat');
load('optimal_WH.mat');
load('optimal_dr.mat');

% 参数设置
D = optimal_D;
W = optimal_WH(1);
H = optimal_WH(2);
h = 6;
dr = optimal_dr;

% 吸收塔坐标 (假设吸收塔位于(0,0)位置，向南移动D米)
absorber_x = 0;
absorber_y = -D;

% 生成定日镜场布局
R = 100:dr:(350+D);
x = []; y = [];
for i = 1:length(R)
    beta = dr/R(i);
    theta = ((-1)^i*beta/4 + asin((R(i)^2+D^2-350^2)/(2*R(i)*D)) + beta/2) : beta : (pi - asin((R(i)^2+D^2-350^2)/(2*R(i)*D)) - beta/4);
    x = [x, R(i)*cos(theta)];
    y = [y, R(i)*sin(theta)];
end

% 创建表格数据
num_heliostats = length(x);
table_data = cell(num_heliostats+1, 8);

% 表头
table_data{1,1} = '吸收塔x坐标 (m)';
table_data{1,2} = '吸收塔y坐标 (m)';
table_data{1,3} = '定日镜序号';
table_data{1,4} = '定日镜宽度 (m)';
table_data{1,5} = '定日镜高度 (m)';
table_data{1,6} = '定日镜x坐标 (m)';
table_data{1,7} = '定日镜y坐标 (m)';
table_data{1,8} = '定日镜z坐标 (m)';

% 吸收塔数据
table_data{2,1} = absorber_x;
table_data{2,2} = absorber_y;

% 定日镜数据
for i = 1:num_heliostats
    table_data{i+2,3} = i;  % 序号
    table_data{i+2,4} = W;  % 宽度
    table_data{i+2,5} = H;  % 高度
    table_data{i+2,6} = x(i);  % x坐标
    table_data{i+2,7} = y(i)-D;  % y坐标 (相对于吸收塔)
    table_data{i+2,8} = h;  % z坐标 (假设所有定日镜高度相同)
end

% 将数据写入Excel文件
filename = 'heliostat_field_layout.xlsx';
writecell(table_data, filename);

disp('表格数据已成功生成并保存为 heliostat_field_layout.xlsx');
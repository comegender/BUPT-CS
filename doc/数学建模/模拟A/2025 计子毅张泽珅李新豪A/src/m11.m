%第一题
clear;
location=readmatrix('fj.xlsx'); %location为定日镜xy坐标
% figure;plot(location(:,1),location(:,2),'.');
%title('附件定日镜坐标信息');axis equal;

loc_jire=[0,0,86]; %集热器中心坐标
h=4; %定日镜高度
loc_dingri=[location,h+zeros(size(location,1),1)]; %定日器中心坐标
s_reflect=loc_jire-loc_dingri; %反射光线的方向向量
s_reflect=s_reflect./sqrt(s_reflect(:,1).^2+s_reflect(:,2).^2+s_reflect(:,3).^2); %单位化

D=readmatrix('日期天数计算.xlsx');
D(:,1:2)=[]; %D为以春分作为第0天起算的天数12个月
ST=8:0.5:10; %当地时间

% i=D长12 j=ST长5共60次
alphas=zeros(12,5);gamas=zeros(12,5);s_in=zeros(12,15); %初始化
phi=39.4*pi/180; %当地纬度

for i=1:12 %第i行为日期
    for j=1:3 %第j列为时间
        delta=asin(sin(2*pi*D(i)/365)*sin(2*pi*23.45/360)); %太阳赤纬角
        w=(ST(j)-12)*pi/12; %太阳时角
        alphas(i,j)=asin(cos(delta)*cos(phi)*cos(w)+sin(delta)*sin(phi)); %太阳高度角
        gamas(i,j)=real(acos((sin(delta)-sin(alphas(i,j))*sin(phi))/(cos(alphas(i,j))*cos(phi)))); %太阳方位角
        
        s_in(i,3*(j-1)+(1:3))=-[sin(gamas(i,j)),cos(gamas(i,j)),tan(alphas(i,j))]; %入射光的方向向量12行*15列每3列一组为一组方向向量
        s_in(i,3*(j-1)+(1:3))=s_in(i,3*(j-1)+(1:3))./norm(s_in(i,3*(j-1)+(1:3))); %单位化
    end
end

s_in(:,10)=-s_in(:,4);
s_in(:,11)=s_in(:,5);
s_in(:,12)=s_in(:,6);
s_in(:,13)=-s_in(:,1);
s_in(:,14)=s_in(:,2);
s_in(:,15)=s_in(:,3);

% i=D长12 j=ST长5共60次
n_dingri=zeros(size(location,1),3); %第i天,第j时,定日器法向量
shade=zeros(12,5,1745); %阴影初始化
ntrunc=zeros(12,5,1745); %截断效率初始化

for i=1:12 %第i行为日期
    for j=1:5 %第j列为时间
        n_dingri=s_in(i,3*(j-1)+(1:3))-s_reflect; %确定每个定日器的法向量
        n_dingri=n_dingri./sqrt(n_dingri(:,1).^2+n_dingri(:,2).^2+n_dingri(:,3).^2);
        
        W=6;H=6; %定日镜W宽度H高度
        v1=[n_dingri(:,2),-n_dingri(:,1),zeros(size(n_dingri(:,1)))];
        v2=[-n_dingri(:,1).*n_dingri(:,3),-n_dingri(:,2).*n_dingri(:,3),n_dingri(:,2).^2+n_dingri(:,1).^2];
        v1=v1./sqrt(v1(:,1).^2+v1(:,3).^2+v1(:,3).^2);
        v2=v2./sqrt(v2(:,1).^2+v2(:,3).^2+v2(:,3).^2);
        
        xp1=location(:,1)+W*v1(:,1)/2+H*v2(:,1)/2;
        yp1=location(:,2)+W*v1(:,2)/2+H*v2(:,2)/2;
        zp1=h+W*v1(:,3)/2+H*v2(:,3)/2;
        
        xp2=location(:,1)-W*v1(:,1)/2+H*v2(:,1)/2;
        yp2=location(:,2)-W*v1(:,2)/2+H*v2(:,2)/2;
        zp2=h-W*v1(:,3)/2+H*v2(:,3)/2;
        
        xp3=location(:,1)-W*v1(:,1)/2-H*v2(:,1)/2;
        yp3=location(:,2)-W*v1(:,2)/2-H*v2(:,2)/2;
        zp3=h-W*v1(:,3)/2-H*v2(:,3)/2;
        
        xp4=location(:,1)+W*v1(:,1)/2-H*v2(:,1)/2;
        yp4=location(:,2)+W*v1(:,2)/2-H*v2(:,2)/2;
        zp4=h+W*v1(:,3)/2-H*v2(:,3)/2;
        
        dl=H/5; %分网格
        xid=W/dl;yid=H/dl; %xy分为xyid格
        
        %%%%%%% 阴影遮挡计算开始
        for k=1:length(xp1) %扫描每一个定日器算阴影
            %从左上角开始向右、向下分格子
            shade1=zeros(xid,yid); %保存第k块板的阴影信息
            
            for ii=1:xid %从左到右一列一列扫
                for jj=1:yid %每一列都从上往下扫
                    xi=xp2(k)+jj*dl*v1(k,1)-ii*dl*v2(k,1)-dl*v1(k,1)/2+dl*v2(k,1)/2;
                    %格子中右下角的坐标作为这一格的代表
                    yi=yp2(k)+jj*dl*v1(k,2)-ii*dl*v2(k,2)-dl*v1(k,2)/2+dl*v2(k,2)/2;
                    zi=zp2(k)+jj*dl*v1(k,3)-ii*dl*v2(k,3)-dl*v1(k,3)/2+dl*v2(k,3)/2;
                    
                    temp=[location,h+zeros(size(location,1),1)]; %临时定日点阵
                    temp(k,:)=[inf,inf,inf];
                    
                    for time=1:6 %找最近的6个点
                        kk=dsearchn(temp,[location(k,:),4]); %kk是temp里距离k最近的定日索引
                        
                        %线面交点计算
                        [px1,py1,pz1]=CalPlaneLineIntersectPoint(n_dingri(kk,:),[location(kk,:),h],s_in(i,3*(j-1)+(1:3)),[xi,yi,zi]);
                        
                        if dot([location(kk,:),h]-[px1,py1,pz1],s_in(i,3*(j-1)+(1:3)))>0
                            result1=is_point_in_rectangular(px1,py1,pz1,[xp1(kk),yp1(kk),zp1(kk);xp2(kk),yp2(kk),zp2(kk);xp3(kk),yp3(kk),zp3(kk);xp4(kk),yp4(kk),zp4(kk)]);
                        else
                            result1=0;
                        end
                        
                        if result1>0
                            shade1(ii,jj)=1;
                            shade(i,j,k)=shade(i,j,k)+1; %找到就退出六次的循环
                            break
                        end
                        
                        [px2,py2,pz2]=CalPlaneLineIntersectPoint(n_dingri(kk,:),[location(kk,:),h],s_reflect(k,:),[xi,yi,zi]);
                        
                        if dot([px2,py2,pz2]-[location(kk,:),h],s_reflect(k,:))>0
                            result2=is_point_in_rectangular(px2,py2,pz2,[xp1(kk),yp1(kk),zp1(kk);xp2(kk),yp2(kk),zp2(kk);xp3(kk),yp3(kk),zp3(kk);xp4(kk),yp4(kk),zp4(kk)]);
                        else
                            result2=0;
                        end
                        
                        if result2>0
                            shade1(ii,jj)=1;
                            shade(i,j,k)=shade(i,j,k)+1; %找到就退出六次的循环
                            break
                        else
                            temp(kk,:)=[inf,inf,inf]; %将找到的最近点删除
                        end
                    end
                end %%%%%%%% 阴影遮挡计算结束
            end %%%%%%%% 没有阴影遮挡的区域 截断计算开始
            
            if sum(sum(shade1))==xid*yid
                ntrunc(i,j,k)=inf;
                continue
            end
            
            d=sqrt(location(k,1)^2+location(k,2)^2+(h-86)^2); %定日镜到集热器中心距离
            r=4.65 * 10^(-3)*d; %光斑半径
            xlimit=4-r;ylimit=4-r;
            light_out=(W+2*r)*(H+2*r); %不考虑阴影的定日镜总输出
            light_in=min(8,2*r+H)*min(8,2*r+W); %不考虑阴影的集热器总输入
            
            [xx,yy]=find(shade1>0); %阴影索引
            for ix=1:length(xx)
                yi=H/2-xx(ix)*dl+dl/2;xi=-W/2+yy(ix)*dl-dl/2;
                
                if xx(ix)==1 && yy(ix)==1 || xx(ix)==1 && yy(ix)==xid || xx(ix)==yid && yy(ix)==1 || xx(ix)==yid && yy(ix)==xid %四个角
                    if r+abs(xi)+dl/2<4 %说明集热器全部吸收
                        light_out=light_out-(dl+r)^2;
                        light_in=light_in-(dl+r)^2;
                    elseif r+abs(yi)+dl/2<4 %说明集热器上下全部吸收 左右溢出
                        light_out=light_out-(dl+r)^2;
                        light_in=light_in-(dl/2+3.5-abs(xi))*(dl+r);
                    else %上下左右都溢出
                        light_out=light_out-(dl+r)^2;
                        light_in=light_in-(dl/2+3.5-abs(xi))*(dl/2+4-abs(yi));
                    end
                elseif xx(ix)==1 && 1<yy(ix) && yy(ix)<xid || xx(ix)==yid && 1<yy(ix) && yy(ix)<xid %左右
                    if r+abs(yi)+dl/2<4 %说明集热器全部吸收
                        light_out=light_out-(dl+r)*dl;
                        light_in=light_in-(dl+r)*dl;
                    else %上下溢出
                        light_out=light_out-(dl+r)*dl;
                        light_in=light_in-(dl/2+4-abs(yi))*dl;
                    end
                elseif yy(ix)==1 && 1<xx(ix) && xx(ix)<yid || yy(ix)==xid && 1<xx(ix) && xx(ix)<yid %上下
                    if r+abs(xi)+dl/2<4 %说明集热器全部吸收
                        light_out=light_out-(dl+r)*dl;
                        light_in=light_in-(dl+r)*dl;
                    else %左右溢出
                        light_out=light_out-(dl+r)*dl;
                        light_in=light_in-(dl/2+3.5-abs(xi))*dl;
                    end
                else
                    light_out=light_out-dl^2;
                    light_in=light_in-dl^2;
                end
            end
            ntrunc(i,j,k)=light_in/light_out;
        end %%%%%%%% 截断计算结束
    end
end

save('Q1.mat')
save('s_in','s_in')
save('alphas','alphas')


%% 子函数
function [px,py,pz]=CalPlaneLineIntersectPoint(en,planepoint,es,linepoint)
% en为法向量 planepoint为平面上一个点的坐标 
% es为直线的方向向量 linepoint为直线上一个点的坐标
vpt=dot(en,es); %内积
if vpt==0
    px=[];py=[];pz=[];
else
    t=((planepoint(1)-linepoint(1))*en(1)+(planepoint(2)-linepoint(2))*en(2)+(planepoint(3)-linepoint(3))*en(3))/vpt;
    px=linepoint(1)+es(1)*t;
    py=linepoint(2)+es(2)*t;
    pz=linepoint(3)+es(3)*t;
end
end

function result = point_in_triangle(px,py,pz,A,B,C)
V1q=[px-A(1),py-A(2),pz-A(3)];
V2q=[px-B(1),py-B(2),pz-B(3)];
V3q=[px-C(1),py-C(2),pz-C(3)];
yon=biggerthan0(B-A,C-A,C-B,V1q,V2q,V3q);
if yon==1 %点在第i个目标面内
    result=1;
else
    result=0;
end
end

% 判断点是否在矩形内部
function result = is_point_in_rectangular(px,py,pz, rectangular)
A = rectangular(1, :);
B = rectangular(2, :);
C = rectangular(3, :);
D = rectangular(4, :);
% 分别判断点是否在两个三角形内部
if point_in_triangle(px,py,pz,A,B,C)||point_in_triangle(px,py,pz,A,C,D)
    result = true;
else
    result = false;
end
end

function yon=biggerthan0(V12,V13,V23,V1q,V2q,V3q)
yon=1;
yon=yon*(dot(cross(V12,V1q),cross(V12,V13))>0);
yon=yon*(dot(cross(V23,V2q),cross(V23,-V12))>0);
yon=yon*(dot(cross(-V13,V3q),cross(-V13,-V23))>0);
end


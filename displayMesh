function [ax] = displayMesh( Mesh, params )
if(nargin < 2)
    params = struct('edgecolor','black','markercolor','green');
end
%trimesh(Mesh.f',Mesh.v(1,:),Mesh.v(2,:),Mesh.v(3,:),'edgecolor', params.edgecolor,'facecolor',params.edgecolor,'facealpha',.3);

if isfield(Mesh, 'vertices'), Mesh.v = Mesh.vertices;, end
if isfield(Mesh, 'faces'), Mesh.f = Mesh.faces;, end

if size(Mesh.v, 1) > size(Mesh.v, 2), Mesh.v = Mesh.v'; end
if size(Mesh.f, 1) > size(Mesh.f, 2), Mesh.f = Mesh.f'; end

grid on;

% axis([-1 1 -1 1 -1 1]);
set(gcf, 'renderer', 'opengl');
% set(gca, 'CameraPosition', [1000 4000 -6000]);
v = [1 1 1];
view(v);

cameratoolbar('Show');
cameratoolbar('SetMode', 'orbit');
cameratoolbar('SetCoordSys', 'z');

xlabel('x');
ylabel('y');
zlabel('z');


[ax] = trisurf(Mesh.f',Mesh.v(1,:),Mesh.v(2,:),Mesh.v(3,:),'edgecolor',params.edgecolor,'edgealpha',.2,'facealpha',1);
hold on;
if(isfield(Mesh,'I'))
    plot3(Mesh.I(:,1),Mesh.I(:,2),Mesh.I(:,3),'x','MarkerEdgeColor',params.markercolor,'MarkerSize',10,'linewidth',3);
end

if(isfield(Mesh,'bb'))
    plot3(Mesh.bb(1,:),Mesh.bb(2,:),Mesh.bb(3,:),'-xr')
    labels = cellstr( num2str([1:size(Mesh.bb,2)]') );
    text(Mesh.bb(1,:),Mesh.bb(2,:),Mesh.bb(3,:), labels, 'VerticalAlignment','bottom', ...
                             'HorizontalAlignment','right')

end

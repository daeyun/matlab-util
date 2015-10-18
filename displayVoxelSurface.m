function displayVoxelSurface(voxels, varargin)
try varargin{:};, catch, varargin={}; end, p=inputParser;
addOptional(p, 'faceColor', 'g');
addOptional(p, 'edgeColor', 'none');
addOptional(p, 'view', [-140 20]);
addOptional(p, 'upAxis', 'z');
parse(p, varargin{:});
args = p.Results;
%%

if size(voxels, 2) == 3
    [V, X, Y, Z] = voxelPointsToGrid(voxels);
else
    [X, Y, Z] = meshgrid(1:size(voxels, 2), 1:size(voxels, 1), 1:size(voxels, 3));
    V = voxels;
end

ptch = patch(isosurface(X, Y, Z, V, 0.5));
isonormals(X, Y, Z, V, ptch);
set(ptch, 'FaceColor', args.faceColor, 'EdgeColor', args.edgeColor);

set(gca,'DataAspectRatio',[1 1 1]);
xlabel('X');
ylabel('Y');
zlabel('Z');
view(args.view(1), args.view(2));
lighting('gouraud');

cameratoolbar('SetCoordSys', args.upAxis);

camlight('headlight');
axis('tight');

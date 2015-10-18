function [V, X, Y, Z] = voxelPointsToGrid(voxelPts, varargin)
try varargin{:};, catch, varargin={}; end, p=inputParser;
addOptional(p,'voxelGridDim',-1);
parse(p, varargin{:});
args = p.Results;

% Find voxel resolution
if(args.voxelGridDim~=-1)
    coord_range = zeros(3,2);
    for i = 1:3
        coord_range(i,:) = [min(voxelPts(:,i)) max(voxelPts(:,i))];
    end
    s = max(abs(coord_range(:,1)-coord_range(:,2)))/args.voxelGridDim;
    ux = [coord_range(1,1)-s: s: coord_range(1,2)+s];
    uy = [coord_range(2,1)-s: s: coord_range(2,2)+s];
    uz = [coord_range(3,1)-s: s: coord_range(3,2)+s];
    
else
    s = Inf;
    for i = 1:3
        dists = sort(unique(voxelPts(:,i)));
        dists = abs(dists - circshift(dists, [1]));
        mindist = min(dists(:));
        if mindist < s
            s = mindist;
        end
    end
    
    ux = unique(voxelPts(:,1));
    uy = unique(voxelPts(:,2));
    uz = unique(voxelPts(:,3));

    ux = [ux(1)-s; ux; ux(end)+s];
    uy = [uy(1)-s; uy; uy(end)+s];
    uz = [uz(1)-s; uz; uz(end)+s];
end




[X,Y,Z] = meshgrid( ux, uy, uz );


V = zeros( size( X ) );
%voxelPts = toGpuArray(voxelPts);

ninds = round(bsxfun(@minus, voxelPts, [ux(1)-s uy(1)-s uz(1)-s])./s);
ninds = sub2ind(size(V), ninds(:,2), ninds(:,1), ninds(:,3));
V(ninds) = 1;

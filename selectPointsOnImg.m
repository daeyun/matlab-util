function [points] = selectPointsOnImg(im, nPoints, varargin)

points = zeros(nPoints, 2);
sz = size(im);

fig = figure;
imshow(im);
title(sprintf('Select %d points on the image.', nPoints), 'FontSize', 17);

hold on;

for i = 1:nPoints
    [x, y] = ginput(1);

    plot([x x], [0 sz(1)]);
    plot([0 sz(2)], [y y]);

    if ismember(varargin, 'verbose')
        if i==1
            fprintf('x, y =\n');
        end
        fprintf('[%4d,%4d]\n', floor(x), floor(y));
    end

    points(i, :) = [x y];
end

close(fig);

end
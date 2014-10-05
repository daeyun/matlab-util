function dispimg(imgCell, varargin)

global defaultFigurePosition;

if isempty(defaultFigurePosition)
    error('run setfigpos first.');
end

if ~strcmp(class(imgCell), 'cell')
    imgCell={imgCell};
end

set(gcf, 'Visible', 'off');

n = numel(imgCell);
cols = ceil(sqrt(n));
rows = round(sqrt(n));

for i = 1:n
    img = imgCell{i};

    subplot(rows, cols, i);
    imshow(img);
end

% [left, bottom, width, height]
p = get(gcf, 'Position');

newLeftMin = defaultFigurePosition(1);
newLeftMax = defaultFigurePosition(1) + p(3);
newBottomMin = defaultFigurePosition(2);
newBottomMax = defaultFigurePosition(2) + p(4);

newBottom = randi([newBottomMin newBottomMax]);
newLeft = randi([newLeftMin newLeftMax]);


if any(ismember(varargin, 'invisible'))
    return
end

set(gcf, 'Position', [newLeft newBottom p(3) p(4)]);
set(gcf, 'Visible', 'on');

set(gcf,'WindowButtonDownFcn', @clickCallback);

    function clickCallback(hObject, ~)
        pos=get(hObject, 'CurrentPoint');
        fprintf('%s\n', mat2str(pos));
    end

end


function setfigpos

f=figure;
set(f,'WindowButtonDownFcn',@windowClickCallback)

function windowClickCallback(~,~)

global defaultFigurePosition
defaultFigurePosition = get(f, 'Position');
set(0, 'DefaultFigurePosition', defaultFigurePosition);
fprintf('default position set to %s\n', mat2str(defaultFigurePosition));
close(f);

end

end
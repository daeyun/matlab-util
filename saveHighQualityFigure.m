% SAVE_HIGH_QUALITY_FIGURE - Save figure as a transparent, lossless, high resolution pdf.
% Requires export_fig: http://www.mathworks.com/matlabcentral/fileexchange/23629-export-fig
function saveHighQualityFigure(fig, path)

if nargin < 1
    timeStr = datestr(clock, 'yyyy-mm-dd-ddd-HH-MM-SS-FFF');
    fig = gcf;
    path = fullfile(pwd, '.', [timeStr '.pdf']);
    disp(path);
end

set(fig, 'Color', 'none');
export_fig(fig, path, '-painters', '-r600', '-q101', '-transparent');
end
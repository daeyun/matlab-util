% EXPORT_FIGURES - Recursively convert all figures in a given directory to image while preserving
% directory structure.
%
% Usage:    savedPath = exportFigures(basePath, figDir, ext, saveDir, isVerbose, isForced)
%
% Arguments:
%     basePath  - path to project root.
%     figDir    - relative path to figures directory. default: 'figures'
%     ext       - file extension. default: 'pdf'
%     saveDir   - relative path to the directory to export the figures.
%                 default: saveDir = ext.
%     saveFunc  - function with the following parameters: (figure, outFile)
%                 default: @saveTightFigure [1].
%     isVerbose - set to true for verbose mode. default: false.
%     isForced  - set to true to save all figures ignoring the midification
%                 date. default: false.
%
% Returns:
%     outPath   - path to the output directory.
% 
% See also:
%     1. http://www.mathworks.com/matlabcentral/fileexchange/34024-get-rid-of-the-white-margin-in-saveas-output/
%     2. http://www.mathworks.com/matlabcentral/fileexchange/32226-recursive-directory-listing-enhanced-rdir
%
% Author:
% Daeyun Shin
% dshin11@illinois.edu  daeyunshin.com
%
% October 2014
function [outPath] = exportFigures(basePath, figDir, ext, saveDir, saveFunc, isVerbose, isForced)
%%
if ~exist('figDir', 'var'), figDir='figures'; end
if ~exist('ext', 'var'), ext='pdf'; end
if ~exist('saveDir', 'var'), saveDir=ext; end
if ~exist('saveFunc', 'var'), saveFunc=@saveTightFigure; end
if ~exist('isVerbose', 'var'), isVerbose=false; end
if ~exist('isForced', 'var'), isForced=false; end

figuresPath = fullfile(basePath, figDir);
outPath = fullfile(basePath, saveDir);

files = rdir(fullfile(figuresPath, '/**/*.fig'));

if isVerbose
    fprintf('%d *.fig files found in %s\n', numel(files), figuresPath);
end

for i = 1:numel(files)
    midPath = regexp(files(i).name, sprintf('%s/(.+?)/(.+?)\\.fig', ...
        figuresPath), 'once', 'ignorecase', 'tokens');
    outDir = fullfile(outPath, midPath{1});
    outFile = fullfile(outDir, [midPath{2} '.' ext]);

    % skip if the existing file is older than the figure file.
    if exist(outFile, 'file') && ~isForced
        fileAtt = rdir(outFile);
        if fileAtt.datenum > files(i).datenum
            if isVerbose
                fprintf('Skipped: %s\n', fileAtt.name);
            end
            continue
        end
    end

    % create (sub)directories if not exists.
    if ~exist(outDir, 'dir')
        mkdir(outDir);
        
        if isVerbose
            fprintf('mkdir -p %s\n', outDir);
        end
    end

    fig = openfig(files(i).name, 'new', 'invisible');
    saveFunc(fig, outFile);
    close(fig);

    if isVerbose
        fprintf('Created: %s\n', outFile);
    end

end

end

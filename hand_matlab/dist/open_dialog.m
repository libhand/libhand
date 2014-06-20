function fullname = open_dialog(varargin)
    [fname, pname] = uigetfile(varargin{:});
    fullname = fullfile(pname, fname);
    
function [joints, rotation] = get_hand_pose(varargin)
    [joints, rotation] = hand_system('get_hand_pose', varargin{:});
    
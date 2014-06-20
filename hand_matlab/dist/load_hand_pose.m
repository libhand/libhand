function [joints, rotation] = load_hand_pose(varargin)
    [joints, rotation] = hand_system('load_hand_pose', varargin{:});
    
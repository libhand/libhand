% Shows off a simple rendering of the hand
%
% Make sure the dist directory is in your MATLAB path
% Run add_paths from this directory if it is not.

% Run hand_system_setup before anything else
hand_system_setup;

% This simply presents a file open dialog so it's easier
% to choose a scene file
scene_file = open_dialog('*.yml', 'Please choose a scene_spec file');

% This command loads the scene in
load_scene(scene_file);

% We render the hand into an image
hand_image = render_hand;

% We tell MATLAB to show the hand image
imshow(hand_image)


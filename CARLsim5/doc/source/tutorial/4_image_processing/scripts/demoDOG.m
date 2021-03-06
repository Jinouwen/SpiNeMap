% OAT demo

%% INITIALIZATION

% init Offline Analysis Toolbox and Visual Stimulus Toolbox
%initOAT


%% CREATING AN IMAGE FILE

% Create a raw stimulus file from an image file using the Visual Stimulus
% Toolbox. CARLsim is able to read raw stimulus files using the Visual
% Stimulus bindings.
%createStimFromImage('../input/carl.jpg', '../input/carl.dat', ...
%   [128 128], 'rgb')


% Run the CARLsim DOG network using the .dat file created above...
% $ make dog
% $ ./dog


%% PLOTTING ACTIVITY

% Plot network activity
clear NM
NM = NetworkMonitor('/home/adarsha/NeuroXplore/toolchain/NeuroXplore/CARLsim5/doc/source/tutorial/4_image_processing/results/sim_dog.dat')
%NM.setGroupPlotType(-1, 'heatmap')
%NM.setGroupPlotType('edges', 'bw')
%NM.addGroup('input','default');
%NM.addGroup('smoothExc','default');
NM.addGroup('edges','heatmap');

NM.plot

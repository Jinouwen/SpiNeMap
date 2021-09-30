% OAT demo

%% INITIALIZATION

% init Offline Analysis Toolbox and Visual Stimulus Toolbox
%initOAT


%% CREATING AN IMAGE FILE

% Create a raw stimulus file from an image file using the Visual Stimulus
% Toolbox. CARLsim is able to read raw stimulus files using the Visual
% Stimulus bindings.
%createStimFromImage('/home/adarsha/NeuroXplore/toolchain/NeuroXplore/CARLsim5/doc/source/tutorial/4_image_processing/input/blue.jpg', '/home/adarsha/NeuroXplore/toolchain/NeuroXplore/CARLsim5/doc/source/tutorial/4_image_processing/input/blue.dat', [128 128], 'rgb')

% Run the CARLsim 'smooth' network using the .dat file created above...
% $ make smooth
% ./smooth


%% PLOTTING ACTIVITY

% Plot network activity
%clear 
NM = NetworkMonitor('/home/adarsha/NeuroXplore/toolchain/NeuroXplore/CARLsim5/doc/source/tutorial/4_image_processing/results/sim_smooth.dat')
%NM.setGroupPlotType('smooth', 'flowfield')
%NM.removeGroup('input');
%NM.addGroup('input','heatmap');
NM.addGroup('smooth','heatmap');
NM.plot
% Name: Jugal Wadhwa
% USC ID: 2979673763
% Email: jwadhwa@usc.edu
% Submission Date: Feb 19th 2024

% Demo for Structured Edge Detector (please see readme.txt first).

%% set opts for training (see edgesTrain.m)
opts=edgesTrain();                % default options (good settings)
opts.modelDir='models/';          % model will be in models/forest
opts.modelFnm='modelBsds';        % model name
opts.nPos=5e5; opts.nNeg=5e5;     % decrease to speedup training
opts.useParfor=0;                 % parallelize if sufficient memory

%% train edge detector (~20m/8Gb per tree, proportional to nPos/nNeg)
tic, model=edgesTrain(opts); toc; % will load model if already trained

%% set detection parameters (can set after training)
model.opts.multiscale=1;          % for top accuracy set multiscale=1
model.opts.sharpen=2;             % for top speed set sharpen=0
model.opts.nTreesEval=4;          % for top speed set nTreesEval=1
model.opts.nThreads=4;            % max number threads for evaluation
model.opts.nms=1;                 % set to true to enable nms

%% evaluate edge detector on BSDS500 (see edgesEval.m)
if(0), edgesEval( model, 'show',1, 'name','' ); end

%% detect edge and visualize results
i_threshold = 0.1;
i_height = 481;
i_width = 321;

I_tiger = imread('../images/Tiger.jpg');
tic, E_tiger=edgesDetect(I_tiger,model); toc
figure(1); im(I_tiger); figure(2); im(1-E_tiger);

% B_tiger = zeros(i_height, i_width);
B_tiger = zeros(i_width, i_height);

for i = 1:i_width
    for j = 1:i_height
        if (E_tiger(i, j) >= i_threshold)
            B_tiger(i, j) = 255;
        end
    end
end

figure(3); imshow(B_tiger);

I_pig = imread('../images/Pig.jpg');
tic, E_pig=edgesDetect(I_pig,model); toc
figure(4); im(I_pig); figure(5); im(1-E_pig)

B_pig = zeros(i_width, i_height);


for i = 1:i_width
    for j = 1:i_height
        if (E_pig(i, j) >= i_threshold)
            B_pig(i, j) = 255;
        end
    end
end

figure(6); imshow(B_pig);

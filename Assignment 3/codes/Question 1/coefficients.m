% Name: Jugal Wadhwa
% USC ID: 2979673763
% Email: jwadhwa@usc.edu
% Submission Date: Mar 10th 2024
t = {
    [0 0; 0 163; 0 327; 81 81; 163 163; 81 244];
    [0 0; 163 0; 327 0; 81 81; 163 163; 244 81];
    [163 163; 244 81; 244 244; 327 0; 327 163; 327 327];
    [0 327; 163 327; 327 327; 81 244; 163 163; 244 244];
};
mt = {
    [0 0; 64 163; 0 327; 81 81; 163 163; 81 244];
    [0 0; 163 64; 327 0; 81 81; 163 163; 244 81];
    [163 163; 244 81; 244 244; 327 0; 263 163; 327 327];
    [0 327; 163 263; 327 327; 81 244; 163 163; 244 244];
};

warp_coefficients = cell(1, 4);

for i=1:4
    original = t{i};
    new = mt{i};

    A = zeros(6,6);
    A(1, :) = 1;
    A(2, :) = original(:, 1);
    A(3, :) = original(:, 2);
    A(4, :) = original(:, 1) .^2;
    A(5, :) = original(:, 1) .* original(:, 2);
    A(6, :) = original(:, 2) .^2;

    warp_coefficients{i} = new' / A;

    disp(warp_coefficients{i})
end

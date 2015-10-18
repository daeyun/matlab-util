function out = renderDepthImage(mesh, camera, imSize)
P = camera.K*[camera.R camera.t];
%% The mex file accepts 0-indexed triangle faces.
out = renderDepthMex(mesh.v, mesh.f-1, imSize, P);

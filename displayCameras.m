function displayCameras(cameras)

hold on;
for i = 1:numel(cameras)
    drawCamera(cameras(i));
end

function drawCamera(camera)

R = camera.R;
t = camera.t;
camPos = (-R'*t)';

drawPoint3d(camPos, 'marker', '*', 'color', 'r', 'markersize', 4)

axes = ([R' -R'*t] * [1 0 0 1;0 1 0 1; 0 0 1 1]')';

drawEdge3d([camPos axes(1,:)], 'color', 'r');
drawEdge3d([camPos axes(2,:)], 'color', 'g');
drawEdge3d([camPos axes(3,:)], 'color', 'b');

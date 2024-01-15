function rotationAngle = GetRotationAngle(t, axisZ, axisX)
    progressionZ = polyfit(t, axisZ, 1);
    progressionX = polyfit(t, axisX, 1);

    rotationAngle = atan2(-progressionZ(1), progressionX(1));
end
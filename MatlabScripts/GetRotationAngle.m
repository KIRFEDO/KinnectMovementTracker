function rotationAngle = GetRotationAngle(t, axisX, axisZ)

    progressionX = polyfit(t, axisX, 1);
    progressionZ = polyfit(t, axisZ, 1);

    rotationAngle = atan2(-progressionX(1), progressionZ(1));
end
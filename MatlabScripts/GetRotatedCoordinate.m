function rotatedCoordinate = GetRotatedCoordinate(angle, currPos, startPos)

    angleVal = angle(1,1);
    cy = cos(angleVal);
    sy = sin(angleVal);
    Ry = [cy  0  -sy;
          0   1  0;
         sy  0  cy];
    rotatedCoordinate = Ry * (currPos)';
    rotatedCoordinate = rotatedCoordinate';
    
end
function rotatedCoordinate = GetRotatedCoordinate(angle, currPos, startPos)

    angleVal = angle(1,1);
    cy = cos(angle);
    sy = sin(angle);
    Ry = [cy  0  sy;
          0   1  0;
         -sy  0  cy];
    rotatedCoordinate = Ry * (currPos - startPos)';
    rotatedCoordinate = rotatedCoordinate';
    
end
// Runs on LM4F120 or TM4C123
// Program written by: Aria Pahlavan and Khalid Qarryzada
// Lab number: 10
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********


DisableInterrupts();	
		int temp;								//orientation- how much to rotate it by? 0 = none, 1 = 90°, etc. 
							//image- whether to print the reg. block or the clearing block
		
	if (y+9 > 159){
		EnableInterrupts();		//enable interrupts before return
		  return 0;}		//0-invalid									//right now the function only draws O-tetromino
	int one,two,three,four;
		y += 18;		//we need to check the place the shape is gonna be next
	if (shapeType == 'O'){
		temp = CheckBoundary(shapeType, orientation, next);
		if (temp == 0){y-=18;  return 0;}	//if CheckBoundary returns a 0, the move is invalid so don't draw.
		
		one = tetState[(((x-9)-1) / 9)][(((y-9)-15) / 9)];
		two = tetState[(((x-9)-1) / 9)+1][(((y-9)-15) / 9)];
		three = tetState[(((x-9)-1) / 9)][(((y-9)-15) / 9)-1];
		four = tetState[(((x-9)-1) / 9)+1][(((y-9)-15) / 9)-1];
	}
	else if (shapeType == 'I'){
		if (orientation ==0){
		temp = CheckBoundary(shapeType, orientation, next);
		if (temp == 0){y-=18;  return 0;}
		one = tetState[(((x-9)-1) / 9)-1][(((y-9)-15) / 9)-1];
		two = tetState[(((x-9)-1) / 9)][(((y-9)-15) / 9)-1];
		three = tetState[(((x-9)-1) / 9)+1][(((y-9)-15) / 9)-1];
		four = tetState[(((x-9)-1) / 9)+2][(((y-9)-15) / 9)-1];
		}
		
		else if (orientation ==1){
		temp = CheckBoundary(shapeType, orientation, next);
		if (temp == 0){y-=18;  return 0;}
		one = tetState[(((x-9)-1) / 9)][(((y-9)-15) / 9)-1];
		two = tetState[(((x-9)-1) / 9)][(((y-9)-15) / 9)];
		three = tetState[(((x-9)-1) / 9)][(((y-9)-15) / 9)+1];
		four = tetState[(((x-9)-1) / 9)][(((y-9)-15) / 9)+2];
		}
	}
	else if (shapeType == 'S'){
		if (orientation ==0){
		temp = CheckBoundary(shapeType, orientation, next);
		if (temp == 0){y-=18;  return 0;}
		one = tetState[(((x-9)-1) / 9)][(((y-9)-15) / 9)];
		two = tetState[(((x-9)-1) / 9)+1][(((y-9)-15) / 9)];
		three = tetState[(((x-9)-1) / 9)+1][(((y-9)-15) / 9)-1];
		four = tetState[(((x-9)-1) / 9)+2][(((y-9)-15) / 9)-1];
		}

		else if(orientation == 1){
		temp = CheckBoundary(shapeType, orientation, next);
		if (temp == 0){y-=18;  return 0;}
		one = tetState[(((x-9)-1) / 9)][(((y-9)-15) / 9)-1];
		two = tetState[(((x-9)-1) / 9)][(((y-9)-15) / 9)];
		three = tetState[(((x-9)-1) / 9)+1][(((y-9)-15) / 9)];
		four = tetState[(((x-9)-1) / 9)+1][(((y-9)-15) / 9)+1];
		}
	}
	else if (shapeType == 'Z'){
		if (orientation == 0){
		temp = CheckBoundary(shapeType, orientation, next);
		if (temp == 0){y-=18;  return 0;}
		one = tetState[(((x-9)-1) / 9)][(((y-9)-15) / 9)-1];
		two = tetState[(((x-9)-1) / 9)+1][(((y-9)-15) / 9)-1];
		three = tetState[(((x-9)-1) / 9)+1][(((y-9)-15) / 9)];
		four = tetState[(((x-9)-1) / 9)+2][(((y-9)-15) / 9)];
		
		}
		
		else if (orientation == 1){
		temp = CheckBoundary(shapeType, orientation, next);
		if (temp == 0){y-=18;  return 0;}
		one = tetState[(((x-9)-1) / 9)+1][(((y-9)-15) / 9)-1];
		two = tetState[(((x-9)-1) / 9)+1][(((y-9)-15) / 9)];
		three = tetState[(((x-9)-1) / 9)][(((y-9)-15) / 9)];
		four = tetState[(((x-9)-1) / 9)][(((y-9)-15) / 9)+1];
		}
	}
	else if (shapeType == 'T'){
		if (orientation == 0){
		temp = CheckBoundary(shapeType, orientation, next);
		if (temp == 0){y-=18;  return 0;}
		one = tetState[(((x-9)-1) / 9)][(((y-9)-15) / 9)-1];
		two = tetState[(((x-9)-1) / 9)+1][(((y-9)-15) / 9)-1];
		three = tetState[(((x-9)-1) / 9)+1][(((y-9)-15) / 9)];
		four = tetState[(((x-9)-1) / 9)+2][(((y-9)-15) / 9)-1];
		}
		else if (orientation == 1){
		temp = CheckBoundary(shapeType, orientation, next);
		if (temp == 0){y-=18;  return 0;}
		one = tetState[(((x-9)-1) / 9)][(((y-9)-15) / 9)];
		two = tetState[(((x-9)-1) / 9)+1][(((y-9)-15) / 9)];
		three = tetState[(((x-9)-1) / 9)+1][(((y-9)-15) / 9)-1];
		four = tetState[(((x-9)-1) / 9)+1][(((y-9)-15) / 9)+1];
		}
		else if (orientation == 2){
		temp = CheckBoundary(shapeType, orientation, next);
		if (temp == 0){y-=18;  return 0;}
		one = tetState[(((x-9)-1) / 9)][(((y-9)-15) / 9)];
		two = tetState[(((x-9)-1) / 9)+1][(((y-9)-15) / 9)];
		three = tetState[(((x-9)-1) / 9)+2][(((y-9)-15) / 9)];
		four = tetState[(((x-9)-1) / 9)+1][(((y-9)-15) / 9)-1];
		}
		else if (orientation ==3){
		temp = CheckBoundary(shapeType, orientation, next);
		if (temp == 0){y-=18;  return 0;}
		one = tetState[(((x-9)-1) / 9)][(((y-9)-15) / 9)-1];
		two = tetState[(((x-9)-1) / 9)][(((y-9)-15) / 9)];
		three = tetState[(((x-9)-1) / 9)][(((y-9)-15) / 9)+1];
		four = tetState[(((x-9)-1) / 9)+1][(((y-9)-15) / 9)];
		}
	}
	else if (shapeType == 'L'){
		if (orientation ==0){
		temp = CheckBoundary(shapeType, orientation, next);
		if (temp == 0){y-=18;  return 0;}
		one = tetState[(((x-9)-1) / 9)][(((y-9)-15) / 9)];
		two = tetState[(((x-9)-1) / 9)][(((y-9)-15) / 9)-1];
		three = tetState[(((x-9)-1) / 9)+1][(((y-9)-15) / 9)-1];
		four = tetState[(((x-9)-1) / 9)+2][(((y-9)-15) / 9)-8];
		}
		else if (orientation ==1){
		temp = CheckBoundary(shapeType, orientation, next);
		if (temp == 0){y-=18;  return 0;}
		one = tetState[(((x-9)-1) / 9)][(((y-9)-15) / 9)-1];
		two = tetState[(((x-9)-1) / 9)+1][(((y-9)-15) / 9)-1];
		three = tetState[(((x-9)-1) / 9)+1][(((y-9)-15) / 9)];
		four = tetState[(((x-9)-1) / 9)+1][(((y-9)-15) / 9)+1];
		}
		else if (orientation ==2){
		temp = CheckBoundary(shapeType, orientation, next);
		if (temp == 0){y-=18;  return 0;}
		one = tetState[(((x-9)-1) / 9)-1][(((y-9)-15) / 9)];
		two = tetState[(((x-9)-1) / 9)][(((y-9)-15) / 9)];
		three = tetState[(((x-9)-1) / 9)+1][(((y-9)-15) / 9)];
		four = tetState[(((x-9)-1) / 9)+1][(((y-9)-15) / 9)-1];
		}
		else if (orientation ==3){
		temp = CheckBoundary(shapeType, orientation, next);
		if (temp == 0){y-=18;  return 0;}
		one = tetState[(((x-9)-1) / 9)][(((y-9)-15) / 9)-1];
		two = tetState[(((x-9)-1) / 9)][(((y-9)-15) / 9)];
		three = tetState[(((x-9)-1) / 9)][(((y-9)-15) / 9)+1];
		four = tetState[(((x-9)-1) / 9)+1][(((y-9)-15) / 9)+1];
		}		
	}
	else if (shapeType == 'J'){
		if (orientation ==0){
		temp = CheckBoundary(shapeType, orientation, next);
		if (temp == 0){y-=18;  return 0;}
		one = tetState[(((x-9)-1) / 9)][(((y-9)-15) / 9)-1];
		two = tetState[(((x-9)-1) / 9)+1][(((y-9)-15) / 9)-1];
		three = tetState[(((x-9)-1) / 9)+2][(((y-9)-15) / 9)-1];
		four = tetState[(((x-9)-1) / 9)+2][(((y-9)-15) / 9)];
		}
		else if (orientation ==1){
		temp = CheckBoundary(shapeType, orientation, next);
		if (temp == 0){y-=18;  return 0;}
		one = tetState[(((x-9)-1) / 9)+1][(((y-9)-15) / 9)-1];
		two = tetState[(((x-9)-1) / 9)+1][(((y-9)-15) / 9)];
		three = tetState[(((x-9)-1) / 9)+1][(((y-9)-15) / 9)+1];
		four = tetState[(((x-9)-1) / 9)][(((y-9)-15) / 9)+1];
		}
		else if (orientation ==2){
		temp = CheckBoundary(shapeType, orientation, next);
		if (temp == 0){y-=18;  return 0;}
		one = tetState[(((x-9)-1) / 9)-1][(((y-9)-15) / 9)-1];
		two = tetState[(((x-9)-1) / 9)-1][(((y-9)-15) / 9)];
		three = tetState[(((x-9)-1) / 9)][(((y-9)-15) / 9)];
		four = tetState[(((x-9)-1) / 9)+1][(((y-9)-15) / 9)];
		}
		else if (orientation ==3){
		temp = CheckBoundary(shapeType, orientation, next);
		if (temp == 0){y-=18;  return 0;}
		one = tetState[(((x-9)-1) / 9)+1][(((y-9)-15) / 9)-1];
		two = tetState[(((x-9)-1) / 9)][(((y-9)-15) / 9)-1];
		three = tetState[(((x-9)-1) / 9)][(((y-9)-15) / 9)];
		four = tetState[(((x-9)-1) / 9)][(((y-9)-15) / 9)+1];
		}		
	}
	else{
		y-=18;  return 0;
	}
	if(one == 1|| two == 1|| three ==1|| four ==1){
		EnableInterrupts();		//enable interrupts before return
		y-=18;  return 0;		//0-invalid
		}
		else{
			EnableInterrupts();
			y-=18;  return 1;		//1-valid
		}
}	


AGSScriptModule    Dualnames AGS Fake Screen module AGS Fake Screen 1.0 �	  // new module script

DynamicSprite*FakeScreen, FakeScreenBG;

int FakeScreenUpdate()
{
  int amount = CalculateThings();
  // draw
  if (FakeScreen!=null) FakeScreen.Delete();
  if (FakeScreenBG!=null) FakeScreenBG.Delete();
  
  
  
  FakeScreen=DynamicSprite.Create(Room.Width, Room.Height, false);
  FakeScreenBG=DynamicSprite.CreateFromBackground(GetBackgroundFrame());
  DrawingSurface *surf = FakeScreen.GetDrawingSurface();
  
  surf.DrawImage(0, 0, FakeScreenBG.Graphic);


  int i = 0;
  while (i < amount)
  {
    int objIndex = Objindex(i);
    int chrIndex = Chrindex(i);
    int wlkIndex = Walkindex(i);
    int baseLine = Baseindex(i);
    if (objIndex != -1)
    {
      int graphic = object[objIndex].Graphic;
      int height = Game.SpriteHeight[graphic];
      if (!object[objIndex].IgnoreScaling)
      {
        int scaling = GetScalingAt(object[objIndex].X, object[objIndex].Y);
        if (scaling != 100)
        {
          height = FloatToInt(IntToFloat(height*scaling)/100.0);
        }
      }
      surf.DrawImage(object[objIndex].X, object[objIndex].Y - height, graphic, object[objIndex].Transparency);

    }
    else if (chrIndex != -1)
    {
      ViewFrame *frame = Game.GetViewFrame(character[chrIndex].View, character[chrIndex].Loop, character[chrIndex].Frame);
      DynamicSprite *sprite;
      int graphic = frame.Graphic;
      if (frame.Flipped)
      {
        sprite = DynamicSprite.CreateFromExistingSprite(graphic);
        sprite.Flip(eFlipLeftToRight);
        graphic = sprite.Graphic;
      }
      int height = FloatToInt(IntToFloat(Game.SpriteHeight[graphic]*character[chrIndex].Scaling)/100.0);
      int width  = FloatToInt(IntToFloat( Game.SpriteWidth[graphic]*character[chrIndex].Scaling)/100.0);

      surf.DrawImage(character[chrIndex].x - width/2, character[chrIndex].y - height - character[chrIndex].z, graphic, character[chrIndex].Transparency, width, height);
    }
    else 
    {
      ReadWalkBehindIntoSprite(FakeScreen.Graphic, FakeScreenBG.Graphic,baseLine); 

    }
    i++;
  }
  surf.Release();
  return FakeScreen.Graphic;
}


Overlay*laylay;

function repeatedly_execute_always()
{
  /*
  game.debug_mode=true;
  Debug(4, 1);
  SetGameSpeed(60);
  int getgraph = FakeScreenUpdate();
  DrawBoxBlur(getgraph, 3);  
  laylay=Overlay.CreateGraphical(0, 0, getgraph, true);
  //*/
}    import int FakeScreenUpdate(); :��L        ej��
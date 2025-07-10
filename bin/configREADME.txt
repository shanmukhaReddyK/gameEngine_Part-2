Window W H FL FS
    -int W width 
    -int H height
    -int FL framelimit
    -bool FS fullscreen or not

Font F S R G B 
    -std::string F filename
    -int S font size
    -int R,G,B color

Player Specification:
Player SR CR S FR FG FB OR OG OB OT V
    -float SR shape radius
    -float CR collosion radius
    -float S speed
    -int FR,FB,FG fill color
    -int OR,OG,OB outline color
    -int OT outline thickness
    -int V Shape vertices

Enemy Specification:
Enemy SR CR SMIN SMAX OR OG OB OT VMIN VMAX L SI 
    -int SR shape radius
    -int CR collosion radius
    -float SMIN,SMAX speed minimum and maximum
    -int OR,OG,OB outline color
    -int OT outline thickness
    -int VMIN,VMAX vertices minimum and maximum
    -int L small lifespan
    -int SI spawn interval

Bullet Specification:
Bullet SR CR S FR FG FB OR OG OB OT V L
    -int SR Shape radius
    -int CR collosion radius
    -float S speed
    -int FR,FG,FB fill color
    -int OR,OG,OB outline color
    -int OT outline thickness
    -int V shape vertices
    -int L lifespan
Window W H FL FS
    -uint W width 
    -uint H height
    -int FL framelimit
    -bool FS fullscreen or not

Font F S R G B 
    -std::string F filename
    -int S font size
    -uint8_t R,G,B color

Player Specification:
Player SR CR S FR FG FB OR OG OB OT V
    -float SR shape radius
    -float CR collosion radius
    -float S speed
    -uint8_t FR,FB,FG fill color
    -uint8_t OR,OG,OB outline color
    -int OT outline thickness
    -int V Shape vertices

Enemy Specification:
Enemy SR CR SMIN SMAX OR OG OB OT VMIN VMAX L SI
    -int SR shape radius
    -int CR collosion radius
    -float SMIN,SMAX speed minimum and maximum
    -uint8_t OR,OG,OB outline color
    -int OT outline thickness
    -int VMIN,VMAX vertices minimum and maximum
    -int L small lifespan
    -int SI spawn interval
    -float S Speed value after randomization

Bullet Specification:
Bullet SR CR S FR FG FB OR OG OB OT V L
    -int SR Shape radius
    -int CR collosion radius
    -float S speed
    -uint8_t FR,FG,FB fill color
    -uint8_t OR,OG,OB outline color
    -int OT outline thickness
    -int V shape vertices
    -int L lifespan
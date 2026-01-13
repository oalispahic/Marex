
var last_pass := 1

loop(var i := 0; i<5; i := i+1)
    loop(var j := 0; j<5; j:=j+1)
        if(j > i)
            print("* ")
        fi
    done
    if(i = 4)
        last_pass := 0
    fi
    
    if(last_pass)
        print(newln)
    fi

done
i := "Test"

var st := 1
var nd := 10

print(i)
print(newln)

loop(st->nd) 
    print(i)
    print(" ")
done


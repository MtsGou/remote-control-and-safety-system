function ParaPerc(param) {

    var PWM = (param * 100) / 4096;
    PWM = Math.round(PWM);
    return PWM;

}

function ParaQualit(param) {

    if (param == "0") {
        var VALUE = "OFF";
    }
    else if (param == "1") {
        var VALUE = "ON";
    }
    return VALUE;
}

var Socket;
function init() {
    Socket = new WebSocket('ws://' + window.location.hostname + ':81/');
    Socket.onmessage = function (event) {
        var data = JSON.parse(event.data);
        console.log(data);

        // ENTRADAS ANALOGICAS

        var decel = data.decel;
        var brake = data.brake;
        var ripio = data.ripio;
        var ripud = data.ripud;
        var llfrg = data.llfrg;
        var lfwbk = data.lfwbk;
        var snsr = data.snsr;
        /*var brsnrl = data.brsnrl; // X
        var brsnrr = data.brsnrr;   // X
        var keystart = data.keystart;*/ //X

        VALORdecel = ParaPerc(decel);
        VALORbrake = ParaPerc(brake);
        VALORripio = ParaPerc(ripio);
        VALORripud = ParaPerc(ripud);
        VALORllfrg = ParaPerc(llfrg);
        VALORlfwbk = ParaPerc(lfwbk);
        VALORsnsr = ParaPerc(snsr);
        /*VALORbrsnrl = ParaPerc(brsnrl); X
        VALORbrsnrr = ParaPerc(brsnrr);   X
        VALORkeystart = ParaPerc(keystart);*/ //X

        document.getElementById("valordecel").innerHTML = VALORdecel;
        document.getElementById("valorbrake").innerHTML = VALORbrake;
        document.getElementById("valorripio").innerHTML = VALORripio;
        document.getElementById("valorripud").innerHTML = VALORripud;
        document.getElementById("valorllfrg").innerHTML = VALORllfrg;
        document.getElementById("valorlfwbk").innerHTML = VALORlfwbk;
        document.getElementById("valorsnsr").innerHTML = VALORsnsr;
        /*document.getElementById("valorbrsnrl").innerHTML = VALORbrsnrl; X
        document.getElementById("valorbrsnrr").innerHTML = VALORbrsnrr;   X
        document.getElementById("valorkeystart").innerHTML = VALORkeystart;*/ //X

        // ENTRADAS DIGITAIS

        var RMMODE = data.RMMODE;
        var swrip = data.swrip;
        var lgat = data.lgat;
        var lpoe = data.lpoe;
        var lpod = data.lpod;
        var lbote = data.lbote;
        var lbotd = data.lbotd;
        var trava = data.trava;

        VALORRMMODE = ParaQualit(RMMODE);
        VALORswrip = ParaQualit(swrip);
        VALORlgat = ParaQualit(lgat);
        VALORlpoe = ParaQualit(lpoe);
        VALORlpod = ParaQualit(lpod);
        VALORlbote = ParaQualit(lbote);
        VALORlbotd = ParaQualit(lbotd);
        VALORtrava = ParaQualit(trava);

        if (data.pbmode == "0") {
            var VALORpbmode = "-";
        }
        else if (data.pbmode == "1") {
            var VALORpbmode = "LOCKED";
        }

        if (data.ups == "0") {
            var VALORups = "OPEN";
        }
        else if (data.ups == "1") {
            var VALORups = "CLOSED";
        }

        /*if (data.dns == "0"){
          var VALORdns = "OPEN";
        }
        else if (data.dns == "1"){
          var VALORdns = "CLOSED";
        }*/

        document.getElementById("valorRMMODE").innerHTML = VALORRMMODE;
        document.getElementById("valorswrip").innerHTML = VALORswrip;
        document.getElementById("valorlgat").innerHTML = VALORlgat;
        document.getElementById("valorlpoe").innerHTML = VALORlpoe;
        document.getElementById("valorlpod").innerHTML = VALORlpod;
        document.getElementById("valorlbote").innerHTML = VALORlbote;
        document.getElementById("valorlbotd").innerHTML = VALORlbotd;
        document.getElementById("valortrava").innerHTML = VALORtrava;
        document.getElementById("valorpbmode").innerHTML = VALORpbmode;
        document.getElementById("valorups").innerHTML = VALORups;
        //document.getElementById("valordns").innerHTML = VALORdns; X

    }
}
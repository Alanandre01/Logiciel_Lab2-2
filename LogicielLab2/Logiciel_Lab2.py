import sys
import keyboard

sys.path.append("C:\\Users\\abrucher\\Downloads\\Logiciel_Lab2-2-master\\x64\\Debug")

import LogicielLab2

global finish
finish = False


def isFinish():
    LogicielLab2.inputChar("q")
    finish = True


def main():

    LogicielLab2.initModule(
        "C:\\Users\\abrucher\\Downloads\\Logiciel_Lab2-2-master\\LogicielLab2\\Example.avi"
    )

    keyboard.on_press_key("p", lambda _: LogicielLab2.inputChar("p"))
    keyboard.on_press_key("r", lambda _: LogicielLab2.inputChar("r"))
    keyboard.on_press_key("s", lambda _: LogicielLab2.inputChar("s"))
    keyboard.on_press_key("q", lambda _: isFinish())

    while not finish:
        pass


if __name__ == "__main__":
    main()

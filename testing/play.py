import chess
import numpy as np
import chess.svg
from PyQt5.QtSvg import QSvgWidget
from PyQt5.QtWidgets import QApplication, QWidget
from helperFuncs import calculateWeight, findBestMove


class MainWindow(QWidget):
    def __init__(self):
        super().__init__()

        self.setGeometry(100, 100, 1000, 1000)

        self.widgetSvg = QSvgWidget(parent=self)
        self.widgetSvg.setGeometry(10, 10, 900, 900)

        self.chessboard = chess.Board()


    def play(self, ai_params):
        while not self.chessboard.is_game_over():
            self.widgetSvg.load(chess.svg.board(self.chessboard, flipped=True).encode("UTF-8"))
            if self.chessboard.turn:
                print("Whites turn")
                self.chessboard.push(findBestMove(ai_params, self.chessboard))
            else:
                print("Blacks turn")
                while True:
                    legal_moves = [str(move) for move in list(self.chessboard.legal_moves)]
                    move = input("Your move: ")
                    if move in legal_moves:
                        self.chessboard.push(chess.Move.from_uci(move))
                        break
                    else:
                        print(move, "This is not a legal move!")
                        print("Legal moves are: ", legal_moves)
        self.widgetSvg.load(chess.svg.board(self.chessboard, flipped=True).encode("UTF-8"))
        print("Game Over")


if __name__ == "__main__":
    ai_params = np.loadtxt("ai_params.txt")
    app = QApplication([])
    window = MainWindow()
    window.show()
    window.play(ai_params)
    app.exec()
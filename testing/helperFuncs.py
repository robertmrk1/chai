import numpy as np
import chess

board_coords = [[chess.A8, chess.B8, chess.C8, chess.D8, chess.E8, chess.F8, chess.G8, chess.H8], 
                [chess.A7, chess.B7, chess.C7, chess.D7, chess.E7, chess.F7, chess.G7, chess.H7],
                [chess.A6, chess.B6, chess.C6, chess.D6, chess.E6, chess.F6, chess.G6, chess.H6],
                [chess.A5, chess.B5, chess.C5, chess.D5, chess.E5, chess.F5, chess.G5, chess.H5],
                [chess.A4, chess.B4, chess.C4, chess.D4, chess.E4, chess.F4, chess.G4, chess.H4],
                [chess.A3, chess.B3, chess.C3, chess.D3, chess.E3, chess.F3, chess.G3, chess.H3],
                [chess.A2, chess.B2, chess.C2, chess.D2, chess.E2, chess.F2, chess.G2, chess.H2],
                [chess.A1, chess.B1, chess.C1, chess.D1, chess.E1, chess.F1, chess.G1, chess.H1]]


def boardToVector(board):
    vector = np.zeros(64, dtype=float)
    k = 0
    for i in range(8):
        for j in range(8):
            if board.color_at(board_coords[i][j]) != None:
                if board.color_at(board_coords[i][j]):
                    vector[k] = board.piece_type_at(board_coords[i][j])
                else:
                    vector[k] = -board.piece_type_at(board_coords[i][j])
            k += 1
    return vector


def calculateWeight(board_vector, ai_params):
    weight = np.dot(board_vector, ai_params[:64]) + ai_params[64]
    if weight < 0:
        weight = 0
    return weight


def findBestMove(ai_params, board):
    best_move   = 0
    best_weight = -10000000.0
    for move in board.legal_moves:
        board.push(move)
        weight = calculateWeight(boardToVector(board), ai_params)
        if weight > best_weight:
            best_move = move
            best_weight = weight
        board.pop()
    return best_move


def play(ai_params1, ai_params2):
    board = chess.Board()
    while not board.is_game_over():
        if board.turn:
            board.push(findBestMove(ai_params1, board))
        else:
            board.push(findBestMove(ai_params2, board)) 
    result = board.result()
    if result == "1-0":
        return 1
    elif result == "1/2-1/2":
        return 0
    else:
        return 2

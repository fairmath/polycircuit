package solution

import (
	"io"

	"github.com/tuneinsight/lattigo/v5/core/rlwe"
	"github.com/tuneinsight/lattigo/v5/he/hefloat"
)

func SolveTestcase(
	params *hefloat.Parameters,
	evl *rlwe.MemEvaluationKeySet,
	pk *rlwe.PublicKey,
	cipher *rlwe.Ciphertext,
	out io.Writer,
) (*rlwe.Ciphertext, error) {
	// Put your solution here
	return nil, nil
}

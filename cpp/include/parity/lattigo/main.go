package main

import (
	"errors"
	"flag"
	"io"
	"log"
	"os"

	"github.com/tuneinsight/lattigo/v5/core/rlwe"
	"github.com/tuneinsight/lattigo/v5/he/hefloat"

	"app/internal/solution"
)

func main() {
	ccFile := flag.String("cc", "", "")
	evalFile := flag.String("key_eval", "", "")
	inputFile := flag.String("input", "", "")
	outputFile := flag.String("output", "", "")
	pkFile := flag.String("key_public", "", "")

	flag.Parse()

	params, err := readContext(ccFile)
	if err != nil {
		log.Fatalf("read context error: %s", err.Error())
	}

	evl, err := readEvaluationKeySet(evalFile)
	if err != nil {
		log.Fatalf("read evaluation key set error: %s", err.Error())
	}

	pk, err := readPublicKey(pkFile)
	if err != nil {
		log.Fatalf("read public key error: %s", err.Error())
	}

	in, err := readInputFile(inputFile)
	if err != nil {
		log.Fatalf("read input file error: %s", err.Error())
	}

	out, err := os.Create(*outputFile)
	if err != nil {
		log.Fatalf("create output file error: %s", err.Error())
	}

	defer out.Close()

	res, err := solution.SolveTestcase(params, evl, pk, in, out)
	if err != nil {
		log.Fatalf("solution error: %s", err)
	}

	if _, err = res.WriteTo(out); err != nil {
		log.Fatalf("can't write resulted ciphertext: %s", err.Error())
	}
}

func readContext(ccFile *string) (*hefloat.Parameters, error) {
	if ccFile == nil {
		return nil, errors.New("ccFile is nil")
	}

	ccData, err := os.ReadFile(*ccFile)
	if err != nil {
		return nil, errors.New("can't read context")
	}

	params := &hefloat.Parameters{}
	if err := params.UnmarshalBinary(ccData); err != nil {
		return nil, errors.New("can't unmarshal context")
	}

	return params, nil
}

func readEvaluationKeySet(evalFile *string) (*rlwe.MemEvaluationKeySet, error) {
	if evalFile == nil {
		return nil, errors.New("evalFile is nil")
	}

	evalF, err := os.Open(*evalFile)
	if err != nil {
		return nil, errors.New("can't open evaluation key set file")
	}
	defer evalF.Close()

	evl := &rlwe.MemEvaluationKeySet{}

	if _, err = evl.ReadFrom(evalF); err != nil && !errors.Is(err, io.EOF) {
		return nil, errors.New("can't read evaluation key set file")
	}

	return evl, nil
}

func readPublicKey(pkFile *string) (*rlwe.PublicKey, error) {
	pkF, err := os.Open(*pkFile)
	if err != nil {
		return nil, errors.New("can't open public key file")
	}
	defer pkF.Close()

	pk := &rlwe.PublicKey{}
	if _, err = pk.ReadFrom(pkF); err != nil && !errors.Is(err, io.EOF) {
		return nil, errors.New("can't read public key file")
	}

	return pk, nil
}

func readInputFile(inputFile *string) (*rlwe.Ciphertext, error) {
	inputF, err := os.Open(*inputFile)
	if err != nil {
		return nil, errors.New("can't open input ciphertext")
	}
	defer inputF.Close()

	in := &rlwe.Ciphertext{}
	if _, err := in.ReadFrom(inputF); err != nil && !errors.Is(err, io.EOF) {
		return nil, errors.New("can't read input ciphertext")
	}

	return in, nil
}

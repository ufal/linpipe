// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

import cz.cuni.mff.ufal.linpipe.Layer;
import cz.cuni.mff.ufal.linpipe.LinPipeError;
import cz.cuni.mff.ufal.linpipe.PlainTextLayer;
import cz.cuni.mff.ufal.linpipe.PlainTextView;

class LinPipeLayers {
  public static void main(String[] args) {
    PlainTextLayer layer = new PlainTextLayer();
    layer.setText(args.length > 0 ? String.join(" ", args) : "Hello, world!");
    System.out.println("Layer as JSON: " + layer.to_json(2));

    PlainTextView view = (PlainTextView) layer.create_view("plain_text");
    System.out.println("Text from a view of type " + view.type() + ": " + view.text());

    Layer copy = Layer.create_from_json(layer.to_json());
    System.out.println("Layer recreated from JSON has class " + copy.getClass().getSimpleName());

    try {
      Layer.create("unknown_type");
    } catch (LinPipeError e) {
      System.out.println("Caught LinPipeError: " + e.getMessage());
    }
  }
}

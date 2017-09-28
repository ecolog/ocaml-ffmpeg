open FFmpeg
open Avutil
open Av

let fill_yuv_image width height frame_index frame =
  (* Y *)
  for y = 0 to height - 1 do
    for x = 0 to width - 1 do
      Video_frame.set frame 0 x y (x + y + frame_index * 3)
    done;
  done;

  (* Cb and Cr *)
  for y = 0 to (height / 2) - 1 do
    for x = 0 to (width / 2) - 1 do
      Video_frame.set frame 1 x y (128 + y + frame_index * 2);
      Video_frame.set frame 2 x y (64 + x + frame_index * 5);
    done;
  done;

  frame


let () =
  if Array.length Sys.argv < 3 then (
    Printf.eprintf "Usage: %s <output file> <codec name>\n" Sys.argv.(0);
    exit 1
  );
  let width = 352 in
  let height = 288 in
  let codec_name = Sys.argv.(2) in
  let pixel_format = Pixel_format.YUV420p in

  let dst = Output.open_file Sys.argv.(1) in

  let sid = Output.new_video_stream ~codec_name ~width ~height ~pixel_format dst in

  let frame = Video_frame.create width height pixel_format in

  for i = 0 to 24 do
    fill_yuv_image width height i frame |> Output.write_video_frame dst sid;
  done;

  Output.close dst;
  Gc.full_major ()
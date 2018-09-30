
use std::io::BufReader;
use std::io::BufRead;
use std::fs::File;
use std::env;

extern crate cairo;

use cairo::Context;
use cairo::ImageSurface;
use cairo::Format;
use cairo::LineCap;
use cairo::LineJoin;


#[derive(Debug)]
struct Data<T> {
	current: T,
	time: T
}


fn load(fnm: &str, l: bool) -> (Vec<Data<f64>>,f64,f64)
{
	println!("Loading {}",fnm);
	let mut dv = Vec::new();
	let f = File::open(fnm).unwrap();
    let file = BufReader::new(&f);
    for line in file.lines() {
    	let l = line.unwrap();
    	let w = l.split_whitespace().collect::<Vec<&str>>();
    	
    	let l4 = &w[0][4..];
    	let w6 = &w[1][6..];
    	
		let c = l4.parse::<i64>().unwrap();
		let t = w6.parse::<i64>().unwrap();
        if t != 0 {
        	dv.push(Data{current:c,time:t});
        }
    }

	let maxt = dv.iter().map(|d|d.time).max().unwrap() as f64;
	let maxc = dv.iter().map(|d|d.current).max().unwrap() as f64;
	let minc = dv.iter().map(|d|d.current).min().unwrap() as f64;

	let kt = 8.* 62.5 / 1000.;
	let rs = 0.5;	// ohm
	let udac = 5.;	// v

//	let kdiv = 11.; // 10k + 1k
//	let kdiv = 6.;  // 10k + 2k
	let kdiv = 14./4.;  // 10k + 4k

	let kc = 1000. * (udac/kdiv) / 255. / rs;
	let u = 100.*14.;	// v
	let mut dv2 = Vec::new();
	for d in &dv {
		if l {
			dv2.push(Data{current:u/((d.current as f64)*kc), time:(d.time as f64)*kt});
		} else {
			dv2.push(Data{current:(d.current as f64)*kc, time:(d.time as f64)*kt});
		}
	}
	
	for d in &dv2 {
		println!("{:?}",d);
	}
	if l {
		(dv2,maxt*kt,u/(minc*kc))
	} else {
		(dv2,maxt*kt,maxc*kc)
	}
}


fn draw_axis(cr: &Context, nx: i32, ny: i32)
{
	cr.set_source_rgb(0.0, 0.0, 0.0);
	cr.move_to(0.0, 0.9);
	cr.line_to(1.0, 0.9);
	cr.move_to(0.1, 0.0);
	cr.line_to(0.1, 1.0);
	
	let stepx = 0.9 / (nx as f64);
	for x in 0..nx {
		cr.move_to(0.1+(x as f64)*stepx, 0.89);
		cr.line_to(0.1+(x as f64)*stepx, 0.91);
	}
	let stepy = 0.9 / (ny as f64);
	for y in 1..ny {
		cr.move_to(0.11, (y as f64)*stepy);
		cr.line_to(0.09, (y as f64)*stepy);
	}
	
	cr.move_to(0.1, 0.0); 	cr.line_to(0.11, 0.02);
	cr.move_to(0.1, 0.0); 	cr.line_to(0.09, 0.02);

	cr.move_to(1.0, 0.9); 	cr.line_to(0.98, 0.89);
	cr.move_to(1.0, 0.9); 	cr.line_to(0.98, 0.91);
	cr.stroke();
	
}


fn draw_grid(cr: &Context, nx: i32, ny: i32)
{
	cr.set_dash(&[0.005,0.01], 0.);
    cr.set_line_cap(LineCap::Round);
    cr.set_line_join(LineJoin::Bevel);
	
	let stepx = 0.9 / (nx as f64);
	for x in 1..nx {
		cr.move_to(0.1+(x as f64)*stepx, 0.01);
		cr.line_to(0.1+(x as f64)*stepx, 0.89);
	}
	let stepy = 0.9 / (ny as f64);
	for y in 1..ny {
		cr.move_to(0.12, (y as f64)*stepy);
		cr.line_to(0.99, (y as f64)*stepy);
	}

	cr.stroke();
	cr.set_dash(&[1.0], 0.);
}


fn draw_metrics(cr: &Context, nx: i32, ny: i32,sx: f64,sy: f64, l:bool)
{
	cr.set_font_size(0.025);

	let stepx = 0.9 / (nx as f64);
	for x in 1..nx {
		cr.move_to(0.1+(x as f64)*stepx, 0.93);
		let val = sx * (x as f64);
		cr.show_text(&format!("{}",val));
	}
	
	let stepy = 0.9 / (ny as f64);
	for y in 1..ny {
		cr.move_to(0.03, 0.9-(y as f64)*stepy);
		let val = sy * (y as f64);
		cr.show_text(&format!("{}",val));
	}
	
	cr.move_to(0.03, 0.03);
	if l {
		cr.show_text("uH");
	} else {
		cr.show_text("ma");
	}
	cr.move_to(0.97, 0.93);
	cr.show_text("us");
}


fn main()
{   
	let mut l = false;	// current or inductance
	let mut series = Vec::new();

	let mut i = 1;
	let mut fnm = env::args().skip(i).next().expect("Missing input file");
	if fnm == "-l" {
	    l = true;
	    i += 1;
	    fnm = env::args().skip(i).next().expect("Missing input file");
	}

 	let (data,mut maxt,mut maxc) = load(&fnm,l);
 	series.push(data);
	i += 1;

	while env::args().len() > i {
		let fnm2 = env::args().skip(i).next().expect("Missing input file");
 		let (data2,maxt2,maxc2) = load(&fnm2,l);
 		if maxc2 > maxc {
 			maxc = maxc2;
 		}
 		if maxt2 > maxt {
 			maxt = maxt2;
 		}
 		series.push(data2);
 		i += 1;
	}


	let w = 800;
	let h = 800;
	let surface = ImageSurface::create(Format::ARgb32, w, h).expect("Can't create surface");
    let cr = Context::new(&surface);
    
    cr.scale(w.into(), h.into());

    cr.set_line_width(0.001);
    cr.set_source_rgb(1.0, 1.0, 1.0);
	cr.rectangle(0.0, 0.0, 1.0, 1.0);
	cr.fill();
  	
  	let mut sx = 10.;
 	let mut nx = 1 + ((maxt/sx).floor() as i32);  	
//	let mut ki = 0;
//	let k = vec![ 2.,2.5.,2.,2.5.,50.,100.,200.,500. ];
	while nx > 14 {
		sx *= 2.;
//		ki += 1;
		nx = 1 + ((maxt/sx).floor() as i32);
	}
	
  	let mut sy = 10.;
  	let mut ny = 1 + ((maxc/sy).floor() as i32);
//	ki = 0;
	while ny > 14 {
		sy *= 2.;
//		ki += 1;
		ny = 1 + ((maxc/sy).floor() as i32);
	}

  	draw_axis(&cr,nx,ny);
  	draw_grid(&cr,nx,ny);
  	draw_metrics(&cr,nx,ny,sx,sy,l);
  	
  	let kx = 0.9 / ((nx as f64) * sx);
  	let ky = 0.9 / ((ny as f64) * sy); 

	let mut count = 0;
	for d in series {
		if count == 0 {
	    	cr.set_source_rgb(1.0, 0.0, 0.0);
	    } else if count == 1 {
	    	cr.set_source_rgb(0.0, 1.0, 0.0);
	    } else {
	    	cr.set_source_rgb(0.0, 0.0, 1.0);
	    }
		for i in 0..d.len()-1 {
  			cr.move_to(0.1+kx*d[i].time, 0.9-ky*d[i].current);
  			cr.line_to(0.1+kx*d[i+1].time, 0.9-ky*d[i+1].current);
  		}
  		count += 1;
	    cr.stroke();
    }
    
	let ofn = fnm+&".png";
    let mut file = File::create(&ofn).expect("Couldn't create output file");
    match surface.write_to_png(&mut file) {
        Ok(_) => println!("{} created",&ofn),
        Err(_) => println!("Error creating {}",&ofn),
	}
}

    
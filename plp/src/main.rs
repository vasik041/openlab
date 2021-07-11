// part list extractor for KiCAD

extern crate quick_xml;
use quick_xml::events::Event;
use quick_xml::Reader;
use quick_xml::Result;
use quick_xml::events::attributes::Attribute;

struct Comp {
    r: String,
    val: String,
    fp: String,
}


fn main() {

    let xml = std::fs::read_to_string("parts.xml").unwrap();

    let mut reader = Reader::from_str(&xml);
    reader.trim_text(true);

    let mut buf = Vec::new();
    let mut comps = Vec::new();

    loop {
        match reader.read_event(&mut buf) {
            Ok(Event::Start(ref e)) if e.name() == b"comp" => {
                let attrs = e.attributes().collect::<Result<Vec<Attribute>>>().unwrap();
                let r = String::from_utf8_lossy(&attrs[0].value);
                let mut val = "".to_string();
                let mut fp = "".to_string();

                let mut buf1 = Vec::new();
                loop {
                    buf1.clear();
                    match reader.read_event(&mut buf1).unwrap() {
                        Event::Start(element) => match element.name() {
                            b"value" => {
                                 let mut txt = Vec::new();
                                 txt.push(reader.read_text(b"value", &mut Vec::new()).expect("Cannot decode text value"));
                                 val = txt[0].clone();
                            }
                            b"footprint" => {
                                 let mut txt = Vec::new();
                                 txt.push(reader.read_text(b"footprint", &mut Vec::new()).expect("Cannot decode text value"));
                                 fp = txt[0].clone();
                            },
                            _ => {},
                        },
                        Event::End(element) => {
                            break;
                        },
                        _ => {},
                    }
                }
                comps.push( Comp { r: r.to_string(), val, fp } );
            }
            Ok(Event::Eof) => break,
            Err(e) => panic!("Error at position {}: {:?}", reader.buffer_position(), e),
            _ => (),
        }
        buf.clear();
    }

    comps.sort_by(|a,b| a.r.cmp(&b.r));
    for c in comps {
        println!("{} {} {}", c.r, c.val, c.fp);
    }
}

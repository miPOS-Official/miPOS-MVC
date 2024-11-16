#define MAX_FILENO 1048576

#include <nodepp/nodepp.h>

#include <nodepp/cluster.h>
#include <express/http.h>
#include <nodepp/query.h>
#include <nodepp/fs.h>

using namespace nodepp;

void onMain() {
  
    if( process::is_parent() ){
      for( int x=os::cpus(); x--; )
         { cluster::add(); }
    }

    auto file = file_t( "./list.txt", 
        fs::exists_file("./list.txt")?"a":"w"
    );
    
    auto app = express::http::add();

    app.POST( "/api", [=]( express_http_t cli ){

        if( cli.headers["Content-Length"].empty() ){ cli.redirect("/"); return; }
        auto len = string::to_ulong( cli.headers["Content-Length"] );

        if( len >= 1024 ){ cli.redirect("/"); return; }
        auto data= query::parse( "?"+url::normalize(cli.read(len)) );

        if( data["email"].empty() ){ cli.redirect("/"); return; }
        file.write( data["email"]+"\n" );

        cli.redirect("/suscribed"); return;

    });

    app.USE( express::http::ssr( "View" ) );

    app.listen( "localhost", 8000, []( ... ){
        console::log( "server started at http://localhost:8000" );
    });

}
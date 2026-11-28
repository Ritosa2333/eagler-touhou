package com.touhouonline.touhouonline

import android.content.Intent
import android.net.Uri
import android.os.Bundle
import io.flutter.embedding.android.FlutterActivity

class MainActivity : FlutterActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        setIntent(intent.withLauncherQuery())
        super.onCreate(savedInstanceState)
    }

    override fun onNewIntent(intent: Intent) {
        super.onNewIntent(intent.withLauncherQuery())
    }

    private fun Intent.withLauncherQuery(): Intent {
        val link = data ?: return this
        if (action != Intent.ACTION_VIEW ||
            link.scheme != "touhouonline" || link.authority != "open" || link.path != "/"
        ) {
            return this
        }

        // Flutter's route observers decode the query before the router can validate it.
        val normalized = Uri.Builder().scheme("touhouonline").authority("open").path("/")
        for (key in listOf("game", "j", "mpRoom", "lobbyAction", "fromLobby")) {
            link.getQueryParameters(key).forEach { normalized.appendQueryParameter(key, it) }
        }
        return Intent(this).setData(normalized.build())
    }
}
